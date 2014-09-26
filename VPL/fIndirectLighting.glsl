#version 430 core

in vec2 pTCoord;

#define PI 3.141593

uniform int VPLCount;
uniform float BounceSingularity;
uniform sampler2D GBufferPositionSampler;
uniform sampler2D GBufferNormalSampler;
uniform sampler2D GBufferAlbedoSampler;

struct VPL
{
    vec4 WorldPosition;
    vec4 WorldNormal;
    vec4 Flux;
};

layout(std430, binding = 0)  buffer _VPLBuffer
{
    VPL vpls[];
} VPLBuffer;

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

    vec3 indirectColor = vec3(0.0, 0.0, 0.0);
    for( int i = 0; i < VPLCount; ++i )
    {
        VPL vpl = VPLBuffer.vpls[i];
        vpl.WorldNormal = vpl.WorldNormal*2.0 - 1.0;

        vec3 incidentDir = PositionWorld.xyz - vpl.WorldPosition.xyz;
        float len = length(incidentDir);
        incidentDir = incidentDir / len;

        float cos0 = max(0.0, dot(NormalWorld, -incidentDir));
        float cos1 = max(0.0, dot(incidentDir, vpl.WorldNormal.xyz));
        float geometricTerm = cos0 * cos1 / max(len*len, BounceSingularity);

        indirectColor += MaterialColor.rgb * vpl.Flux.rgb * geometricTerm;
    }

    indirectColor = indirectColor * 2 * PI;
    gl_FragData[0] = vec4(indirectColor, 1.0);
}
