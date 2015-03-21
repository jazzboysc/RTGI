#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

#include "VPLviaSVOGI/sSparseVoxelOctree.glsl"

void main()
{
    voxelFragmentBuffer.count = atomicCounter(voxelFragmentCounter);
    voxelFragmentBuffer.instanceCount = 1;
    voxelFragmentBuffer.first = 0;
    voxelFragmentBuffer.baseInstance = 0;
}