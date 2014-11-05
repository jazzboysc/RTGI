#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Voxel
{
    uint radiance;
};

layout(std430, binding = 0)  buffer gpuMemoryPool
{
    Voxel data[];
} voxelBuffer;

void main()
{
    int index = int(gl_GlobalInvocationID.z * gl_NumWorkGroups.y * gl_NumWorkGroups.x + 
        gl_GlobalInvocationID.y * gl_NumWorkGroups.x + gl_GlobalInvocationID.x);
    voxelBuffer.data[index].radiance = 0;
}