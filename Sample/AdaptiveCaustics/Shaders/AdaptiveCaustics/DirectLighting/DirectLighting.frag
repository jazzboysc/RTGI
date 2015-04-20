#version 430 core

#include "AdaptiveCaustics/DirectLighting/SceneLight.glsl"

in vec2 pTCoord;

out vec4 Output;

uniform sampler2D GBufferPositionSampler;
uniform sampler2D GBufferNormalSampler;
uniform sampler2D GBufferAlbedoSampler;
uniform sampler2D CausticMapSampler;
uniform sampler2D CausticMapDepthSampler;

/*
vec4 CausticContribution( sampler2DShadow cDepth, sampler2D cMap, vec4 smapCoord )
{
	return ( all(equal(smapCoord.xyz,clamp(smapCoord.xyz,vec3(0),vec3(1)))) ? 
					shadow2D( cDepth, smapCoord.xyz ).x * texture2D( cMap, smapCoord.xy ): 
					vec4(0.0) );
}
*/

vec4 CausticContribution( vec4 WorldPos)
{
	SceneLight light = sceneLightUniformBuffer.lights[0];
	vec4 viewPos = light.LightProjectorView * WorldPos;
	viewPos = viewPos * 0.5 + 0.5;
	return texture(CausticMapSampler, viewPos.xy);
}

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

    Output = accumulation + CausticContribution(PositionWorld);
}
