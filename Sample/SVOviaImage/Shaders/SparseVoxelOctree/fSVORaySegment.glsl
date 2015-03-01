#version 430 core

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    if( svoCommandBuffer.hit > 0 )
    {
        gl_FragData[0] = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        gl_FragData[0] = vec4(0.5, 0.5, 1.0, 1.0);
    }
}