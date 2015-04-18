#version 430 core

#include "AdaptiveCaustics/DirectLighting/SceneLight.glsl"

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
    //NormalWorld = NormalWorld*2.0 - 1.0;

    vec4 Material = texture(GBufferAlbedoSampler, pTCoord);

    vec4 accumulation = vec4(0.0, 0.0, 0.0, 0.0);

    uint pointLightCount = sceneLightUniformBuffer.info.PointLightCount;
    for( int i = 0; i < pointLightCount; ++i )
    {
        accumulation += ComputePointLight(i, PositionWorld, NormalWorld, Material);
    }

    uint spotLightCount = sceneLightUniformBuffer.info.SpotLightCount;
    for( int i = 0; i < spotLightCount; ++i )
    {
        accumulation += ComputeSpotLight(i, PositionWorld, NormalWorld, Material);
    }

    Output = accumulation;
}
