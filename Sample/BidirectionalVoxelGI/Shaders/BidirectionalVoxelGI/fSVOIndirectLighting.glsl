#version 430 core

#include "BidirectionalVoxelGI/sSparseVoxelOctree.glsl"

in vec2 pTCoord;

out vec4 Output;

#define PI 3.141593

uniform int VPLCount;
uniform int PatternSize;
uniform float BounceSingularity;
uniform sampler2D GBufferPositionSampler;
uniform sampler2D GBufferNormalSampler;
uniform sampler2D GBufferAlbedoSampler;
uniform vec3 SceneBBCenter;
uniform vec3 SceneBBExtension;
uniform int dim;

struct VPL
{
    vec4 WorldPosition;
    vec4 WorldNormal;
    vec4 Flux;
    mat4 View;
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

    int patternIndex = int(gl_FragCoord.x) % PatternSize + (int(gl_FragCoord.y) % PatternSize) * PatternSize;
    int sampleVPLCount = VPLCount / (PatternSize*PatternSize);
    int vplBufferIndex = sampleVPLCount * patternIndex;

    // Initialize SVO root node.
    SVONode root;
    root.info = SVO_NODE_FLAG_MASK;
    root.nodeBox.Min = Ivec3ToUint(ivec3(0, 0, 0));
    root.nodeBox.Max = Ivec3ToUint(ivec3(SVO_MAX_LEVEL_DIM,
        SVO_MAX_LEVEL_DIM, SVO_MAX_LEVEL_DIM));

    vec3 indirectColor = vec3(0.0, 0.0, 0.0);
    for( int i = 0; i < sampleVPLCount; ++i )
    {
        // Fetch VPL.
        VPL vpl = VPLBuffer.vpls[vplBufferIndex + i];

        // VPL visibility test.
        uint vTerm = SVOIntersectionTest(PositionWorld.xyz, vpl.WorldPosition.xyz, root);

        vpl.WorldNormal = vpl.WorldNormal*2.0 - 1.0;

        vec3 incidentDir = PositionWorld.xyz - vpl.WorldPosition.xyz;
        float len = length(incidentDir);
        incidentDir = incidentDir / len;

        // Compute G term.
        float cos0 = max(0.0, dot(NormalWorld, -incidentDir));
        float cos1 = max(0.0, dot(incidentDir, vpl.WorldNormal.xyz));
        float geometricTerm = cos0 * cos1 / max(len*len, BounceSingularity);

        // Accumulate VPL's contribution.
        indirectColor += vpl.Flux.rgb * geometricTerm * (1 - vTerm);
    }

    indirectColor = MaterialColor.rgb * indirectColor * 2 * PI;
    Output = vec4(indirectColor, 1.0);
}
