#version 410 core

in vec2 pTCoord;

uniform mat4 VPLProjectorView;
uniform vec3 VPLPositionWorld;
uniform vec3 VPLNormalWorld;
uniform vec3 VPLColor;
uniform vec2 VPLProjectorNearFar;

uniform sampler2D GBufferPositionSampler;
uniform sampler2D GBufferNormalSampler;
uniform sampler2D GBufferDiffuseSampler;
uniform sampler2D VPLShadowMapSampler;

void main()
{
    vec3 position = texture(GBufferPositionSampler, pTCoord).rgb;
    vec3 normal = texture(GBufferNormalSampler, pTCoord).rgb;
    normal = normal*2.0 - 1.0;
    vec3 diffuse = texture(GBufferDiffuseSampler, pTCoord).rgb;

    vec3 test = VPLPositionWorld - position;
    float testLen = length(test);
    if( testLen <= 0.1 )
    {
        gl_FragData[0] = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
        bool skipShadow = false;
        vec4 viewPos = VPLProjectorView * vec4(position, 1.0);
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
        float currDepth = (len - VPLProjectorNearFar.x) /
            (VPLProjectorNearFar.y - VPLProjectorNearFar.x);

        vec2 texCoords = vec2(u, v);
        texCoords = texCoords*0.5 + 0.5;
        float depth = texture(VPLShadowMapSampler, texCoords).r;

        if( currDepth - depth > 0.01 && !skipShadow )
        {
            // Indirect shadow.
            gl_FragData[0] = vec4(0.0, 0.0, 0.0, 1.0);
        }
        else
        {
            // Indirect lighting.
            vec3 lightDir = VPLPositionWorld - position;
            float len = length(lightDir);
            lightDir = lightDir / len;
            float d0 = max(0.0, dot(lightDir, normal));
            float d1 = max(0.0, dot(-lightDir, VPLNormalWorld));
            vec3 color = diffuse * VPLColor * d0 * d1 * 10.0/ len;
            gl_FragData[0] = vec4(color, 1.0);
        }
    }
}
