#version 430 core

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    if( gl_VertexID == 0 )
    {
        svoNodeBuffer.rootFlag = 0;
    }

    svoNodeBuffer.data[gl_VertexID].child = 0;
    svoNodeBuffer.data[gl_VertexID].flag = 0;
}