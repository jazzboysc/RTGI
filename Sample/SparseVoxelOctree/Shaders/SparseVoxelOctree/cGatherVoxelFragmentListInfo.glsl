#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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

void main()
{
    voxelFragmentBuffer.count = atomicCounter(voxelFragmentCounter);
    voxelFragmentBuffer.instanceCount = 1;
    voxelFragmentBuffer.first = 0;
    voxelFragmentBuffer.baseInstance = 0;
}