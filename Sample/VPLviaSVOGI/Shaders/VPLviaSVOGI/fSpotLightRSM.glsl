#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

in vec4 gPositionWorld;
in vec4 gNormalWorld;

out vec4[3] Output;

uniform vec3 MaterialColor;

void main()
{
    vec3 normal = normalize(gNormalWorld.xyz);

    uint curLightIndex = rsmUniformBuffer.info.CurLightIndex;
    SceneLight light = sceneLightUniformBuffer.lights[curLightIndex];

    vec3 lightDir = light.WorldPositionAndType.xyz - gPositionWorld.xyz;
    float len = length(lightDir);
    lightDir = lightDir / len;

    float attenuation = 0.0;
    float spotEffect = dot(light.Params1.xyz, -lightDir);
    float spotCosCutoff = light.Params1.w;
    float spotInnerCosCutoff = light.Params2.w;
    float spotExponent = light.Params2.x;
    float constantAttenuation = light.Params2.y;
    float quadraticAttenuation = light.Params2.z;

    vec3 luminance = vec3(0.0, 0.0, 0.0);
    if( spotEffect >= spotCosCutoff )
    {
        float falloff = clamp((spotEffect - spotInnerCosCutoff) /
            (spotInnerCosCutoff - spotCosCutoff), 0.0, 1.0);

        attenuation = falloff * pow(spotEffect, spotExponent) /
            (constantAttenuation + quadraticAttenuation*len*len);
        float d = max(0.0, dot(lightDir, normal));
        luminance = MaterialColor.rgb * light.Intensity.xyz * d;
    }

    Output[0] = gPositionWorld;
    normal.xyz = (normal.xyz + 1.0) * 0.5;
    Output[1] = vec4(normal, 0.0);
    Output[2] = vec4(luminance, 1.0);
}