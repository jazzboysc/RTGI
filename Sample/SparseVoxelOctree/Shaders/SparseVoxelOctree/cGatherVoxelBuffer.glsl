#version 430 core

#define LOCAL_GROUP_DIM 8
layout(local_size_x = LOCAL_GROUP_DIM, local_size_y = LOCAL_GROUP_DIM, local_size_z = LOCAL_GROUP_DIM) in;

#include "SparseVoxelOctree/sVoxelGrid.glsl"

layout(binding = 0, offset = 4) uniform atomic_uint gpuMemoryAllocator;

uniform vec3 SceneBBMin;
uniform vec3 SceneBBExtension;

void main()
{
    int index = int(gl_GlobalInvocationID.z * gl_NumWorkGroups.y * gl_NumWorkGroups.x * LOCAL_GROUP_DIM * LOCAL_GROUP_DIM +
        gl_GlobalInvocationID.y * gl_NumWorkGroups.x * LOCAL_GROUP_DIM + gl_GlobalInvocationID.x);

    vec3 voxelSize = 2.0*SceneBBExtension / vec3(gl_NumWorkGroups * LOCAL_GROUP_DIM);
    vec3 voxelWorldPosition = SceneBBMin + vec3(gl_GlobalInvocationID)*voxelSize + 0.5*voxelSize;

    if( index == 0 )
    {
        indirectCommandBuffer.primCount = 0;

        // Debug voxel world space size.
        indirectCommandBuffer.reserved1 = voxelSize.x;
        indirectCommandBuffer.reserved2 = voxelSize.y;
        indirectCommandBuffer.reserved3 = voxelSize.z;
        indirectCommandBuffer.reserved4 = -1.0;
        indirectCommandBuffer.reserved5 = -1.0;
        indirectCommandBuffer.reserved6 = -1.0;
        indirectCommandBuffer.reserved7 = -1.0;
        indirectCommandBuffer.reserved8 = -1.0;
        indirectCommandBuffer.reserved9 = -1.0;
        indirectCommandBuffer.reserved10 = -1.0;
        indirectCommandBuffer.reserved11 = -1.0;
        indirectCommandBuffer.reserved12 = -1.0;
        indirectCommandBuffer.reserved13 = -1.0;
        indirectCommandBuffer.reserved14 = -1.0;
        indirectCommandBuffer.reserved15 = -1.0;
        indirectCommandBuffer.reserved16 = -1.0;
        indirectCommandBuffer.reserved17 = -1.0;
        indirectCommandBuffer.reserved18 = -1.0;
        indirectCommandBuffer.reserved19 = -1.0;
        indirectCommandBuffer.reserved20 = -1.0;
        indirectCommandBuffer.reserved21 = -1.0;
        indirectCommandBuffer.reserved22 = -1.0;
        indirectCommandBuffer.reserved23 = -1.0;
        indirectCommandBuffer.reserved24 = -1.0;
        indirectCommandBuffer.reserved25 = -1.0;
        indirectCommandBuffer.reserved26 = -1.0;
        indirectCommandBuffer.reserved27 = -1.0;
        indirectCommandBuffer.reserved28 = -1.0;
        indirectCommandBuffer.reserved29 = -1.0;
        indirectCommandBuffer.reserved30 = -1.0;
        indirectCommandBuffer.reserved31 = -1.0;
        indirectCommandBuffer.reserved32 = -1.0;
        indirectCommandBuffer.reserved33 = -1.0;
        indirectCommandBuffer.reserved34 = -1.0;
        indirectCommandBuffer.reserved35 = -1.0;
    }

    memoryBarrier();

    if( voxelBuffer.data[index].value2 > 0 )
    {
        uint newLocation = atomicCounterIncrement(gpuMemoryAllocator);
        GatheredVoxel data;
        data.translation = vec4(voxelWorldPosition, 1.0);
        indirectCommandBuffer.data[newLocation] = data;

        atomicAdd(indirectCommandBuffer.primCount, 1);
    }
}