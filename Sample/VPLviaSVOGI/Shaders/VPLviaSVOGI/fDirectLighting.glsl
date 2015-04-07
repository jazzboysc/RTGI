#version 430 core

#include "VPLviaSVOGI/sSceneLight.glsl"

in vec2 pTCoord;

out vec4 Output;

uniform mat4 LightProjectorView;
uniform vec3 LightPositionWorld;
uniform vec3 LightColor;
uniform vec2 LightProjectorNearFar;
uniform bool ShowShadow;

uniform sampler2D GBufferPositionSampler;
uniform sampler2D GBufferNormalSampler;
uniform sampler2D GBufferAlbedoSampler;
uniform sampler2D ShadowMapSampler;

void main()
{
    vec4 PositionWorld = texture(GBufferPositionSampler, pTCoord);
    if( PositionWorld.w == 0.0 )
    {
        discard;
    }

    vec3 NormalWorld = texture(GBufferNormalSampler, pTCoord).rgb;
    NormalWorld = NormalWorld*2.0 - 1.0;

    vec4 MaterialColor = texture(GBufferAlbedoSampler, pTCoord);

    // Compare with shadow map bias.
    bool skipShadow = false;
    vec4 viewPos = LightProjectorView * PositionWorld;
    if( viewPos.z > -0.05 )
    {
        skipShadow = true;
    }
    vec3 viewDir = viewPos.xyz;
    float len = length(viewDir);
    viewDir = normalize(viewDir);
    vec3 halfDir = viewDir + vec3(0.0, 0.0, -1.0);
    float u = -halfDir.x / halfDir.z;
    float v = -halfDir.y / halfDir.z;
    float currDepth = (len - LightProjectorNearFar.x) /
        (LightProjectorNearFar.y - LightProjectorNearFar.x);

    vec2 texCoords = vec2(u, v);
    texCoords = texCoords*0.5 + 0.5;
    float depth = texture(ShadowMapSampler, texCoords).r;

    if( currDepth - depth > 0.01 && !skipShadow && ShowShadow )
    {
        // Shadow.
        Output = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // Direct lighting.
        vec3 lightDir = sceneLightUniformBuffer.lights[0].WorldPositionAndType.xyz - PositionWorld.xyz;
        float len = length(lightDir);
        lightDir = lightDir / len;
        float d = max(0.0, dot(lightDir, NormalWorld));
        vec3 color = MaterialColor.rgb * sceneLightUniformBuffer.lights[0].Intensity.xyz * d / (1.0 + 0.4*len*len);
        Output = vec4(color, 1.0);
    }
}
