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
    uint  count;
    uint  instanceCount;
    uint  first;
    uint  baseInstance;

    VoxelFragment data[]; // must be big enough to hold all voxel fragments.
} voxelFragmentBuffer;

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
uint GetSVOChildNodeIndex()
{
    return 0;
}
//----------------------------------------------------------------------------