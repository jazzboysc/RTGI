#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Voxel
{
    uint value1;
    uint value2;
    uint value3;
    uint value4;
};

layout(std430, binding = 0)  buffer gpuMemoryPool
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
} indirectCommandBuffer;

void main()
{
    int index = int(gl_GlobalInvocationID.z * gl_NumWorkGroups.y * gl_NumWorkGroups.x + 
        gl_GlobalInvocationID.y * gl_NumWorkGroups.x + gl_GlobalInvocationID.x);

    if( voxelBuffer.data[index].value2 > 0 )
    {
        atomicAdd(indirectCommandBuffer.primCount, 1);
    }
}