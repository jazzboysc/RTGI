#version 430 core

#define LOCAL_GROUP_DIM 8
layout(local_size_x = LOCAL_GROUP_DIM, local_size_y = LOCAL_GROUP_DIM, local_size_z = LOCAL_GROUP_DIM) in;

struct Voxel
{
    uint value1;
    uint value2;
    uint value3;
    uint value4;
};

struct GatheredVoxel
{
    vec4 translation;
};

layout(std430, binding = 0)  buffer _voxelBuffer
{
    Voxel data[];
} voxelBuffer;

layout(std430, binding = 1)  buffer _indirectCommandBuffer
{
    uint  count;
    uint  primCount;
    uint  firstIndex;
    uint  baseVertex;
    uint  baseInstance;

    float reserved1;
    float reserved2;
    float reserved3;

    GatheredVoxel data[];
} indirectCommandBuffer;

layout(binding = 0, offset = 0) uniform atomic_uint gpuMemoryAllocator;

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