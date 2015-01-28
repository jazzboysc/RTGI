//----------------------------------------------------------------------------
// Sparse Voxel Octree
//----------------------------------------------------------------------------
layout(binding = 0, offset = 4) uniform atomic_uint svoNodeAllocator;

struct SVONode
{
    uint child;
    uint flag;
};

layout(std430, binding = 3)  buffer _svoNodeBuffer
{
    SVONode data[];
} svoNodeBuffer;