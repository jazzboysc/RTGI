//----------------------------------------------------------------------------
// Sparse Voxel Octree
//----------------------------------------------------------------------------
layout(binding = 0, offset = 4) uniform atomic_uint svoNodeAllocator;

#define SVO_NODE_TILE_SIZE 8
#define SVO_NODE_FLAGED 1234

struct SVONode
{
    uint child;
    uint flag;
};

struct SVONodeAABB
{
    ivec3 Min;
    ivec3 Max;
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

    // SVO node buffer. Must be big enough to hold all tree nodes.
    SVONode data[];
} svoNodeBuffer;

layout(std140, binding = 0) uniform _svoUniformBuffer
{
    uint curLevel;
    uint dim;
} svoUniformBuffer;

//----------------------------------------------------------------------------
uint GetSVOChildNodeIndex(ivec3 voxelGridPos, SVONodeAABB nodeBox)
{
    ivec3 mid = (nodeBox.Min + nodeBox.Max) >> 1;
    uint childIndex = (voxelGridPos.x >= mid.x ? 4 : 0) +
                      (voxelGridPos.y >= mid.y ? 2 : 0) +
                      (voxelGridPos.z >= mid.z ? 1 : 0);
    return childIndex;
}
//----------------------------------------------------------------------------
SVONodeAABB GetSVOChildNodeBox(uint childIndex, SVONodeAABB nodeBox)
{
    SVONodeAABB childNodeBoxes[8];

    ivec3 mid = (nodeBox.Min + nodeBox.Max) >> 1;
    childNodeBoxes[0].Min = nodeBox.Min;
    childNodeBoxes[0].Max = mid;
    childNodeBoxes[1].Min = ivec3(nodeBox.Min.x, nodeBox.Min.y, mid.z);
    childNodeBoxes[1].Max = ivec3(mid.x, mid.y, nodeBox.Max.z);
    childNodeBoxes[2].Min = ivec3(nodeBox.Min.x, mid.y, nodeBox.Min.z);
    childNodeBoxes[2].Max = ivec3(mid.x, nodeBox.Max.y, mid.z);
    childNodeBoxes[3].Min = ivec3(nodeBox.Min.x, mid.y, mid.z);
    childNodeBoxes[3].Max = ivec3(mid.x, nodeBox.Max.y, nodeBox.Max.z);
    childNodeBoxes[4].Min = ivec3(mid.x, nodeBox.Min.y, nodeBox.Min.z);
    childNodeBoxes[4].Max = ivec3(nodeBox.Max.x, mid.y, mid.z);
    childNodeBoxes[5].Min = ivec3(mid.x, nodeBox.Min.y, mid.z);
    childNodeBoxes[5].Max = ivec3(nodeBox.Max.x, mid.y, nodeBox.Max.z);
    childNodeBoxes[6].Min = ivec3(mid.x, mid.y, nodeBox.Min.z);
    childNodeBoxes[6].Max = ivec3(nodeBox.Max.x, nodeBox.Max.y, mid.z);
    childNodeBoxes[7].Min = mid;
    childNodeBoxes[7].Max = nodeBox.Max;

    return childNodeBoxes[childIndex];
}
//----------------------------------------------------------------------------