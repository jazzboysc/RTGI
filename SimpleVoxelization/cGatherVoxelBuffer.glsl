#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

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
    uint  reserved1;
    uint  reserved2;
    uint  reserved3;
    GatheredVoxel data[];
} indirectCommandBuffer;

layout(binding = 0, offset = 0) uniform atomic_uint gpuMemoryAllocator;

void main()
{
    int index = int(gl_GlobalInvocationID.z * gl_NumWorkGroups.y * gl_NumWorkGroups.x + 
        gl_GlobalInvocationID.y * gl_NumWorkGroups.x + gl_GlobalInvocationID.x);

    if( index == 0 )
    {
        indirectCommandBuffer.count = 0;
        indirectCommandBuffer.primCount = 0;
        indirectCommandBuffer.firstIndex = 0;
        indirectCommandBuffer.baseVertex = 0;
        indirectCommandBuffer.baseInstance = 0;
    }

    memoryBarrier();

    if( voxelBuffer.data[index].value2 > 0 )
    {
        uint newLocation = atomicCounterIncrement(gpuMemoryAllocator);
        GatheredVoxel data;
        data.translation = vec4(float(gl_GlobalInvocationID.x), 
                                float(gl_GlobalInvocationID.y), 
                                float(gl_GlobalInvocationID.z), 
                                1.0);
        indirectCommandBuffer.data[newLocation] = data;

        atomicAdd(indirectCommandBuffer.primCount, 1);
    }
}