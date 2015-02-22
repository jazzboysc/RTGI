#define SVO_NODE_TILE_SIZE 8
#define SVO_NODE_FLAGED 1234
#define SVO_MAX_LEVEL 7
#define SVO_MAX_LEVEL_DIM 128

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

    // Scene bounding box.
    vec4 SceneBBMin;
    vec4 SceneBBCenter;
    vec4 SceneBBExtension;
    vec4 Inv2SceneBBExtension;

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
    uint isLeaf;
    uint albedo;

    SVONodeAABB nodeBox;
};

layout(std430, binding = 3)  buffer _svoNodeBuffer
{
    // Indirect command buffer data for allocation of SVO node tile pass.
    uint  allocThreadCountForCurLevel;
    uint  instanceCount;
    uint  first;
    uint  baseInstance;

    // Indirect command buffer data for SVO visualization.
    uint  ic2Count;
    uint  ic2PrimCount;
    uint  ic2FirstIndex;
    uint  ic2BaseVertex;
    uint  ic2BaseInstance;

    uint offset0;
    uint offset1;
    uint offset2;

    // Shared data between SVO passes.
    uint rootFlag;
    uint rootChild;
    uint curLevelStartIndex;
    uint curLevelEndIndex;

    // Debug.
    uint hit;
    float minT;
    float maxT;
    float sceneMaxT;
    uint isLeaf;
    uint childIndex;
    uint childTileIndex;
    uint nextNodeIndex;
    float tNear;
    float tFar;
    float minMinusOriginK;
    float maxMinusOriginK;
    uint tNearCompareTFar;
    uint b;
    uint c;
    uint d;
    vec4 sceneBBMin;
    vec4 rayStartPos;
    vec4 rayStartPosSVO;
    vec4 rayEndPos;
    vec4 rayEndPosSVO;
    vec4 rayEntryPos;
    vec4 nodeBoxMin;
    vec4 nodeBoxMax;
    vec4 mid;
    vec4 rayInvDirSVO;

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
uint Vec4ToUint(vec4 value)
{
    uint res = (uint(value.w) & 0x000000FF) << 24U |
        (uint(value.z) & 0x000000FF) << 16U |
        (uint(value.y) & 0x000000FF) << 8U |
        (uint(value.x) & 0x000000FF);

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
uint GetSVOChildNodeIndex2(vec3 svoSpaceP, SVONodeAABB nodeBox)
{
    vec3 nodeBoxMin, nodeBoxMax, mid;
    nodeBoxMin = vec3(UintToIvec3(nodeBox.Min));
    nodeBoxMax = vec3(UintToIvec3(nodeBox.Max));
    mid = (nodeBoxMin + nodeBoxMax)*0.5;

    uint childIndex = (svoSpaceP.x >= mid.x ? 4 : 0) +
                      (svoSpaceP.y >= mid.y ? 2 : 0) +
                      (svoSpaceP.z >= mid.z ? 1 : 0);
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
bool IsSVOLeafNode(SVONode node)
{
    return (node.isLeaf == 1);
}
//----------------------------------------------------------------------------
void InitSVONode(uint nodeIndex)
{
    svoNodeBuffer.data[nodeIndex].child = 0;
    svoNodeBuffer.data[nodeIndex].flag = 0;
    svoNodeBuffer.data[nodeIndex].isLeaf = 1;
    svoNodeBuffer.data[nodeIndex].albedo = 0;
}
//----------------------------------------------------------------------------
ivec3 WorldToGridPosition(vec3 worldPosition)
{
    vec3 gridDim = vec3(float(SVO_MAX_LEVEL_DIM), float(SVO_MAX_LEVEL_DIM), 
        float(SVO_MAX_LEVEL_DIM));
    gridDim = gridDim - vec3(1.0, 1.0, 1.0);

    vec3 offsets = (worldPosition - voxelFragmentBuffer.SceneBBCenter.xyz + 
        voxelFragmentBuffer.SceneBBExtension.xyz) * 
        voxelFragmentBuffer.Inv2SceneBBExtension.xyz;
    offsets = offsets*gridDim + vec3(0.5, 0.5, 0.5);
    ivec3 res = ivec3(offsets);

    return res;
}
//----------------------------------------------------------------------------