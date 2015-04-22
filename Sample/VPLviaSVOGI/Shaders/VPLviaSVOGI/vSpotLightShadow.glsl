#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

in vec4 vPosition;

out float vViewPosZ;

uniform mat4 World;

void main()
{
    // Fetch light info.
    uint curLightIndex = shadowMapUniformBuffer.info.CurLightIndex;
    SceneLight curLight = sceneLightUniformBuffer.lights[curLightIndex];

    vec4 viewPos = curLight.LightProjectorView * World * vPosition;
    vViewPosZ = (-viewPos.z - curLight.LightProjectorNearFar.x) / (curLight.LightProjectorNearFar.y - curLight.LightProjectorNearFar.x);
    gl_Position = curLight.LightProjectorProj * viewPos;
}