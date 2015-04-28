#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

layout (triangles) in;
layout (triangle_strip) out;

in vec4 vPositionWorld[];
in vec4 vNormalWorld[];

out vec4 gPositionWorld;
out vec4 gNormalWorld;

mat4 GetFaceViewTransform(vec3 lightPositionWorld, int face)
{
    const vec3 PX = vec3(1.0, 0.0, 0.0);
    const vec3 NX = vec3(-1.0, 0.0, 0.0);
    const vec3 PY = vec3(0.0, 1.0, 0.0);
    const vec3 NY = vec3(0.0, -1.0, 0.0);
    const vec3 PZ = vec3(0.0, 0.0, 1.0);
    const vec3 NZ = vec3(0.0, 0.0, -1.0);
    const float EdPX = dot(lightPositionWorld, PX);
    const float EdNX = dot(lightPositionWorld, NX);
    const float EdPY = dot(lightPositionWorld, PY);
    const float EdNY = dot(lightPositionWorld, NY);
    const float EdPZ = dot(lightPositionWorld, PZ);
    const float EdNZ = dot(lightPositionWorld, NZ);

    mat4 res;
    if( face == 0 )
    {
        // face PX.
        res = mat4(vec4(PZ.x, PY.x, NX.x, 0.0),
                   vec4(PZ.y, PY.y, NX.y, 0.0),
                   vec4(PZ.z, PY.z, NX.z, 0.0),
                   vec4(-EdPZ, -EdPY, -EdNX, 1.0));
    }
    else if( face == 1 )
    {
        // face NX.
        res = mat4(vec4(NZ.x, PY.x, PX.x, 0.0),
                   vec4(NZ.y, PY.y, PX.y, 0.0),
                   vec4(NZ.z, PY.z, PX.z, 0.0),
                   vec4(-EdNZ, -EdPY, -EdPX, 1.0));
    }
    else if( face == 2 )
    {
        // face PY.
        res = mat4(vec4(PX.x, PZ.x, NY.x, 0.0),
                   vec4(PX.y, PZ.y, NY.y, 0.0),
                   vec4(PX.z, PZ.z, NY.z, 0.0),
                   vec4(-EdPX, -EdPZ, -EdNY, 1.0));
    }
    else if( face == 3 )
    {
        // face NY.
        res = mat4(vec4(PX.x, NZ.x, PY.x, 0.0),
                   vec4(PX.y, NZ.y, PY.y, 0.0),
                   vec4(PX.z, NZ.z, PY.z, 0.0),
                   vec4(-EdPX, -EdNZ, -EdPY, 1.0));
    }
    else if( face == 4 )
    {
        // face NZ.
        res = mat4(vec4(PX.x, PY.x, PZ.x, 0.0),
                   vec4(PX.y, PY.y, PZ.y, 0.0),
                   vec4(PX.z, PY.z, PZ.z, 0.0),
                   vec4(-EdPX, -EdPY, -EdPZ, 1.0));
    }

    return res;
}

void main()
{
    // Fetch light info.
    uint curLightIndex = rsmUniformBuffer.info.CurLightIndex;
    SceneLight curLight = sceneLightUniformBuffer.lights[curLightIndex];

    mat4 View, ProjView;
    const int faceCount = 5;

    for( int i = 0; i < faceCount; ++i )
    {
        View = GetFaceViewTransform(curLight.WorldPositionAndType.xyz, i);
        ProjView = curLight.LightProjectorProj * View;

        for( int j = 0; j < gl_in.length(); ++j )
        {
            gPositionWorld = vPositionWorld[j];
            gNormalWorld = vNormalWorld[j];

            gl_Layer = i + int(curLightIndex);
            gl_Position = ProjView * gl_in[j].gl_Position;

            EmitVertex();
        }

        EndPrimitive();
    }
}