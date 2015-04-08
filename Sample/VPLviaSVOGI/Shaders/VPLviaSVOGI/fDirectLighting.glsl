#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

in vec2 pTCoord;

out vec4 Output;

uniform sampler2D GBufferPositionSampler;
uniform sampler2D GBufferNormalSampler;
uniform sampler2D GBufferAlbedoSampler;

uniform int PointLightCount;
uniform int SpotLightCount;

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
    for( int i = 0; i < PointLightCount; ++i )
    {
        accumulation += ComputePointLight(i, PositionWorld, NormalWorld, Material);
    }
    for( int i = 0; i < SpotLightCount; ++i )
    {
        accumulation += ComputeSpotLight(i, PositionWorld, NormalWorld, Material);
    }

    Output = accumulation;
}
