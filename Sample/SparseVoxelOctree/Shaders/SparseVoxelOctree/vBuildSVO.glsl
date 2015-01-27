#version 430 core

//----------------------------------------------------------------------------
// SVO Voxel Fragment List
//----------------------------------------------------------------------------
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
    voxelFragmentBuffer.data[gl_VertexID].value1 = gl_VertexID;
    voxelFragmentBuffer.data[gl_VertexID].value2 = gl_InstanceID + 1;
}