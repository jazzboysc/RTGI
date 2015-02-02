#define SVO_NODE_TILE_SIZE 8
#define SVO_NODE_FLAGED 1234

//----------------------------------------------------------------------------
// SVO Voxel Fragment List
//----------------------------------------------------------------------------
layout(binding = 0, offset = 0) uniform atomic_uint voxelFragmentCounter;

struct VoxelFragment
{
    uint gridPosition;
    uint albedo;
    uint value1;
    uint value2;
};

layout(std430, binding = 1)  buffer _voxelFragmentBuffer
{
    // Indirect command buffer data for SVO flag nodes pass.
    uint  count;
    uint  instanceCount;
    uint  first;
    uint  baseInstance;

    // Voxel fragment buffer. Must be big enough to hold all voxel fragments.
    VoxelFragment data[];
} voxelFragmentBuffer;

//----------------------------------------------------------------------------
// Sparse Voxel Octree
//----------------------------------------------------------------------------
layout(binding = 0, offset = 4) uniform atomic_uint svoNodeAllocator;

struct SVONodeAABB
{
    uint Min;
    uint Max;
};

struct SVONode
{
    uint child;
    uint flag;

    SVONodeAABB nodeBox;
};

layout(std430, binding = 3)  buffer _svoNodeBuffer
{
    // Indirect command buffer data for allocation of SVO node tile pass.
    uint  allocThreadCountForCurLevel;
    uint  instanceCount;
    uint  first;
    uint  baseInstance;

    // Shared data between SVO passes.
    uint rootFlag;
    uint rootChild;
    uint curLevelStartIndex;
    uint curLevelEndIndex;

    // Debug.
    uint value1;
    uint value2;
    uint value3;
    uint value4;

    // SVO node buffer. Must be big enough to hold all tree nodes.
    SVONode data[];
} svoNodeBuffer;

layout(std140, binding = 0) uniform _svoUniformBuffer
{
    uint curLevel;
    uint dim;
} svoUniformBuffer;

//----------------------------------------------------------------------------
uint Ivec3ToUint(ivec3 value)
{
    uint res = (uint(value.z) & 0x000003FF) << 20U |
               (uint(value.y) & 0x000003FF) << 10U |
               (uint(value.x) & 0x000003FF);

    return res;
}
//----------------------------------------------------------------------------
ivec3 UintToIvec3(uint value)
{
    ivec3 res = ivec3(int(value & 0x000003FF),
                      int((value & 0x000FFC00) >> 10U),
                      int((value & 0x3FF00000) >> 20U));

    return res;
}
//----------------------------------------------------------------------------
uint GetSVOChildNodeIndex(ivec3 voxelGridPos, SVONodeAABB nodeBox)
{
    ivec3 nodeBoxMin, nodeBoxMax, mid;
    nodeBoxMin = UintToIvec3(nodeBox.Min);
    nodeBoxMax = UintToIvec3(nodeBox.Max);
    mid = (nodeBoxMin + nodeBoxMax) >> 1;

    uint childIndex = (voxelGridPos.x >= mid.x ? 4 : 0) +
                      (voxelGridPos.y >= mid.y ? 2 : 0) +
                      (voxelGridPos.z >= mid.z ? 1 : 0);
    return childIndex;
}
//----------------------------------------------------------------------------
SVONodeAABB GetSVOChildNodeBox(uint childIndex, SVONodeAABB nodeBox)
{
    SVONodeAABB childNodeBoxes[8];

    ivec3 nodeBoxMin, nodeBoxMax, mid;
    nodeBoxMin = UintToIvec3(nodeBox.Min);
    nodeBoxMax = UintToIvec3(nodeBox.Max);
    mid = (nodeBoxMin + nodeBoxMax) >> 1;

    childNodeBoxes[0].Min = Ivec3ToUint(nodeBoxMin);
    childNodeBoxes[0].Max = Ivec3ToUint(mid);
    childNodeBoxes[1].Min = Ivec3ToUint(ivec3(nodeBoxMin.x, nodeBoxMin.y, mid.z));
    childNodeBoxes[1].Max = Ivec3ToUint(ivec3(mid.x, mid.y, nodeBoxMax.z));
    childNodeBoxes[2].Min = Ivec3ToUint(ivec3(nodeBoxMin.x, mid.y, nodeBoxMin.z));
    childNodeBoxes[2].Max = Ivec3ToUint(ivec3(mid.x, nodeBoxMax.y, mid.z));
    childNodeBoxes[3].Min = Ivec3ToUint(ivec3(nodeBoxMin.x, mid.y, mid.z));
    childNodeBoxes[3].Max = Ivec3ToUint(ivec3(mid.x, nodeBoxMax.y, nodeBoxMax.z));
    childNodeBoxes[4].Min = Ivec3ToUint(ivec3(mid.x, nodeBoxMin.y, nodeBoxMin.z));
    childNodeBoxes[4].Max = Ivec3ToUint(ivec3(nodeBoxMax.x, mid.y, mid.z));
    childNodeBoxes[5].Min = Ivec3ToUint(ivec3(mid.x, nodeBoxMin.y, mid.z));
    childNodeBoxes[5].Max = Ivec3ToUint(ivec3(nodeBoxMax.x, mid.y, nodeBoxMax.z));
    childNodeBoxes[6].Min = Ivec3ToUint(ivec3(mid.x, mid.y, nodeBoxMin.z));
    childNodeBoxes[6].Max = Ivec3ToUint(ivec3(nodeBoxMax.x, nodeBoxMax.y, mid.z));
    childNodeBoxes[7].Min = Ivec3ToUint(mid);
    childNodeBoxes[7].Max = Ivec3ToUint(nodeBoxMax);

    return childNodeBoxes[childIndex];
}
//----------------------------------------------------------------------------
void FlagSVONode(uint nodeIndex)
{
    svoNodeBuffer.data[nodeIndex].flag = SVO_NODE_FLAGED;
}
//----------------------------------------------------------------------------
bool IsSVONodeFlaged(uint nodeIndex)
{
    return (svoNodeBuffer.data[nodeIndex].flag == SVO_NODE_FLAGED);
}
//----------------------------------------------------------------------------