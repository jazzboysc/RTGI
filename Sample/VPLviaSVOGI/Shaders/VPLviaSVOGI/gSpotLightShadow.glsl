#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in float vViewPosZ[];

out float gViewPosZ;

void main()
{
    uint curLightIndex = shadowMapUniformBuffer.info.CurLightIndex;

    for( int i = 0; i < gl_in.length(); ++i )
    {
        gViewPosZ = vViewPosZ[i];
        gl_Position = gl_in[i].gl_Position;

        gl_Layer = int(curLightIndex);
        EmitVertex();
    }

    EndPrimitive();
}