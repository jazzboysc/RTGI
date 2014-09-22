#version 410 core

in vec2 pTCoord;

uniform mat4 LightProjectorView;
uniform vec3 LightPositionWorld;
uniform vec3 LightColor;
uniform vec2 LightProjectorNearFar;

uniform sampler2D GBufferPositionSampler;
uniform sampler2D GBufferNormalSampler;
uniform sampler2D GBufferAlbedoSampler;
uniform sampler2D ShadowMapSampler;

void main()
{
    vec4 PositionWorld = texture(GBufferPositionSampler, pTCoord);

    vec3 NormalWorld = texture(GBufferNormalSampler, pTCoord).xyz;
    NormalWorld = NormalWorld*2.0 - 1.0;

    vec4 MaterialColor = texture(GBufferAlbedoSampler, pTCoord);
    if( MaterialColor.rgb == vec3(0.5, 0.5, 0.5) )
    {
        gl_FragData[0] = vec4(1.0, 0.0, 0.0, 1.0);
        return;
    }

    bool skipShadow = false;
    vec4 viewPos = LightProjectorView * PositionWorld;
    if( viewPos.z > 0.0 )
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

    if( currDepth - depth > 0.01 && !skipShadow )
    {
        // Shadow.
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // Direct lighting.
        vec3 lightDir = LightPositionWorld - PositionWorld.xyz;
        lightDir = normalize(lightDir);
        float d = max(0.0, dot(lightDir, NormalWorld));
        vec3 color = MaterialColor.rgb * d;
        gl_FragData[0] = vec4(color, 1.0);
    }
}
