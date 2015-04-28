#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

in vec4 vPosition;
in vec3 vNormal;

out vec4 vPositionWorld;
out vec4 vNormalWorld;

uniform mat4 World;

void main()
{
    // Fetch light info.
    uint curLightIndex = rsmUniformBuffer.info.CurLightIndex;
    SceneLight curLight = sceneLightUniformBuffer.lights[curLightIndex];

    vPositionWorld = World * vPosition;
    vNormalWorld = World * vec4(vNormal, 0.0);
    gl_Position = curLight.LightProjectorProj * curLight.LightProjectorView * vPositionWorld;
}
