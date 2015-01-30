//----------------------------------------------------------------------------
// Sparse Voxel Octree
//----------------------------------------------------------------------------
layout(binding = 0, offset = 4) uniform atomic_uint svoNodeAllocator;

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
    uint rootFlag;
    uint rootChild;
    uint value1;
    uint value2;

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
    uint childIndex = (voxelGridPos.x > mid.x ? 4 : 0) +
                      (voxelGridPos.y > mid.y ? 2 : 0) +
                      (voxelGridPos.z > mid.z ? 1 : 0);
    return childIndex;
}
//----------------------------------------------------------------------------