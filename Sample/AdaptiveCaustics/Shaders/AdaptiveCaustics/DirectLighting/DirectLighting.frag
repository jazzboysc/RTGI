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

vec4 CausticContribution(vec4 WorldPos, vec3 NormalPos)
{
	SceneLight light = sceneLightUniformBuffer.lights[0];
	vec3 toLight0   = normalize( light.WorldPositionAndType.xyz - WorldPos.xyz );
	float NdotL0    = max( 0.0, dot( NormalPos.xyz, toLight0 ) );
	vec4 viewPos = light.LightProjectorView * WorldPos;
	viewPos = viewPos * 0.5 + 0.5;

	/*
	vec3 viewDir = viewPos.xyz;
    float len = length(viewDir);
    viewDir = normalize(viewDir);
    vec3 halfDir = viewDir + vec3(0.0, 0.0, -1.0);
    float u = -halfDir.x / halfDir.z;
    float v = -halfDir.y / halfDir.z;
    float currDepth = (len - light.LightProjectorNearFar.x) /
        (light.LightProjectorNearFar.y - light.LightProjectorNearFar.x);

    vec2 texCoords = vec2(u, v);
    texCoords = texCoords*0.5 + 0.5;
	*/
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

    Output = accumulation + vec4(CausticContribution(PositionWorld, NormalWorld).xyz, 0.0f);
}
