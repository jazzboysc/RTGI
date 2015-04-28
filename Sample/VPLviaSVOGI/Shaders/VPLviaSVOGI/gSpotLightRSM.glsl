#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 vPositionWorld[];
in vec4 vNormalWorld[];

out vec4 gPositionWorld;
out vec4 gNormalWorld;

void main()
{
    uint curLightIndex = rsmUniformBuffer.info.CurLightIndex;

    for( int i = 0; i < gl_in.length(); ++i )
    {
        gPositionWorld = vPositionWorld[i];
        gNormalWorld = vNormalWorld[i];
        gl_Position = gl_in[i].gl_Position;

        gl_Layer = int(curLightIndex);
        EmitVertex();
    }

    EndPrimitive();
}