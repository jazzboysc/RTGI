#version 430 core

#include "SparseVoxelOctree/sVoxelFragmentList.glsl"
#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    voxelFragmentBuffer.data[gl_VertexID].value1 = gl_VertexID;
    voxelFragmentBuffer.data[gl_VertexID].value2 = gl_InstanceID + 1;
}