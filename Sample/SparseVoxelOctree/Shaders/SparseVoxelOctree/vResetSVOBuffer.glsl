#version 430 core

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    svoNodeBuffer.data[gl_VertexID].flag = 0;
}