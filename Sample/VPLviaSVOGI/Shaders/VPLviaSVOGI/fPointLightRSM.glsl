#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

in vec4 gPositionWorld;
in vec4 gNormalWorld;

out vec4[3] Output;

uniform vec3 MaterialColor;

void main()
{
    // Fetch light info.
    uint curLightIndex = rsmUniformBuffer.info.CurLightIndex;
    SceneLight curLight = sceneLightUniformBuffer.lights[curLightIndex];

    vec3 normal = normalize(gNormalWorld.xyz);
    normal.xyz = (normal.xyz + 1.0) * 0.5;
    Output[0] = gPositionWorld;
    Output[1] = vec4(normal, 0.0);
    Output[2] = vec4(MaterialColor*curLight.Intensity.xyz, 1.0);
}