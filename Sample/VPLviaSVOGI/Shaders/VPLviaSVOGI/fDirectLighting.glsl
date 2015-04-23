#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

in vec2 pTCoord;

out vec4 Output;

uniform sampler2D GBufferPositionSampler;
uniform sampler2D GBufferNormalSampler;
uniform sampler2D GBufferAlbedoSampler;

void main()
{
    vec4 PositionWorld = texture(GBufferPositionSampler, pTCoord);
    if( PositionWorld.w == 0.0 )
    {
        discard;
    }

    vec3 NormalWorld = texture(GBufferNormalSampler, pTCoord).rgb;
    NormalWorld = NormalWorld*2.0 - 1.0;

    vec4 Material = texture(GBufferAlbedoSampler, pTCoord);

    vec4 accumulation = vec4(0.0, 0.0, 0.0, 0.0);

    uint pointLightCount = sceneLightUniformBuffer.info.PointLightCount;
    int i = 0;
    for( ; i < pointLightCount; ++i )
    {
        accumulation += ComputePointLight(i, PositionWorld, NormalWorld, Material);
    }

    uint spotLightCount = sceneLightUniformBuffer.info.SpotLightCount;
    for( int j = 0; j < spotLightCount; ++j )
    {
        accumulation += ComputeSpotLight(i + j, PositionWorld, NormalWorld, Material);
    }

    accumulation /= (pointLightCount + spotLightCount);
    Output = accumulation;
}
