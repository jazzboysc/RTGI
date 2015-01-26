#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

//----------------------------------------------------------------------------
// SVO Voxel Fragment List
//----------------------------------------------------------------------------
layout(binding = 0, offset = 0) uniform atomic_uint voxelFragmentCounter;

struct VoxelFragment
{
    uint xyz;
    uint albedo;
};

layout(std430, binding = 1)  buffer _voxelFragmentBuffer
{
    uint dispatchX;  // == voxelFragmentCounter
    uint dispatchY;  // == 1
    uint dispatchZ;  // == 1
    uint value1;

    VoxelFragment data[]; // must be big enough to hold all voxel fragments.
} voxelFragmentBuffer;
//----------------------------------------------------------------------------

void main()
{
    voxelFragmentBuffer.dispatchX = atomicCounter(voxelFragmentCounter);
    voxelFragmentBuffer.dispatchY = 1;
    voxelFragmentBuffer.dispatchZ = 1;
}