#version 430 core

#define LOCAL_GROUP_DIM 8
layout(local_size_x = LOCAL_GROUP_DIM, local_size_y = LOCAL_GROUP_DIM, local_size_z = LOCAL_GROUP_DIM) in;

#include "SparseVoxelOctree/sVoxelGrid.glsl"

void main()
{
    int index = int(gl_GlobalInvocationID.z * gl_NumWorkGroups.y * gl_NumWorkGroups.x * LOCAL_GROUP_DIM * LOCAL_GROUP_DIM +
        gl_GlobalInvocationID.y * gl_NumWorkGroups.x * LOCAL_GROUP_DIM + gl_GlobalInvocationID.x);
    voxelBuffer.data[index].value1 = 0;
    voxelBuffer.data[index].value2 = 0;
    voxelBuffer.data[index].value3 = 0;
    voxelBuffer.data[index].value4 = 0;
}