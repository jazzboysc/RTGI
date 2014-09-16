#version 410 core

in vec2 pTCoord;

uniform mat4 LightProjectorView;
uniform vec3 LightPositionWorld;
uniform vec3 LightColor;
uniform vec2 LightProjectorNearFar;

uniform sampler2D GBufferPositionSampler;
uniform sampler2D GBufferNormalSampler;
uniform sampler2D GBufferDiffuseSampler;
uniform sampler2D ShadowMapSampler;

void main()
{
    vec3 position = texture(GBufferPositionSampler, pTCoord).rgb;
    vec3 normal = texture(GBufferNormalSampler, pTCoord).rgb;
    normal = normal*2.0 - 1.0;
    vec3 diffuse = texture(GBufferDiffuseSampler, pTCoord).rgb;

    bool skipShadow = false;
    vec4 viewPos = LightProjectorView * vec4(position, 1.0);
    if( viewPos.z >= 0.0 )
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

    if( currDepth - depth > 0.001 && !skipShadow )
    {
        // Shadow.
        gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        // Direct lighting.
        vec3 lightDir = LightPositionWorld - position.xyz;
        lightDir = normalize(lightDir);
        float d = max(0.0, dot(lightDir, normal));
        vec3 color = diffuse * d;
        gl_FragData[0] = vec4(color, 1.0);
    }
}
