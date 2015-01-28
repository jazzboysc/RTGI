#version 430 core

#include "SparseVoxelOctree/sVoxelGrid.glsl"

void main()
{
    if( indirectCommandBuffer.reserved4 > 0.0 )
    {
        gl_FragData[0] = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        gl_FragData[0] = vec4(0.0, 0.0, 1.0, 1.0);
    }
}