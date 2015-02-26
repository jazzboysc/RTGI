#version 430 core

#include "BidirectionalVoxelGI/sSparseVoxelOctree.glsl"

in vec2 pTCoord;

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
uniform bool VPLVisibilityTest;

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

bool SVOIntersectionTest(vec3 rayStartPoint, vec3 rayEndPoint)
{
    float minT, maxT, sceneMinT, sceneMaxT;

    // Transform world space ray to SVO space.
    vec3 sceneBBMin = voxelFragmentBuffer.SceneBBMin.xyz;
    vec3 inv2SceneBBExtension = voxelFragmentBuffer.Inv2SceneBBExtension.xyz;
    vec3 rayStartPosSVO = (rayStartPoint - sceneBBMin)*inv2SceneBBExtension*SVO_MAX_LEVEL_DIM;
    vec3 rayEndPosSVO = (rayEndPoint - sceneBBMin)*inv2SceneBBExtension*SVO_MAX_LEVEL_DIM;

    // Initialize sceneMinT, sceneMaxT and ray direction.
    vec3 rayDirSVO = rayEndPosSVO - rayStartPosSVO;
    sceneMinT = 0.0;
    sceneMaxT = length(rayDirSVO);
    rayDirSVO = rayDirSVO / sceneMaxT;
    rayStartPosSVO = rayStartPosSVO + rayDirSVO*2.5;
    rayEndPosSVO = rayEndPosSVO - rayDirSVO*2.5;
    vec3 rayInvDirSVO = 1.0 / rayDirSVO;

    bool hit = false;
    minT = maxT = sceneMinT;
    SVONode curNode, root;
    vec3 rayEntryPos;

    // Initialize root node.
    root.info = SVO_NODE_FLAG_MASK;
    root.nodeBox.Min = Ivec3ToUint(ivec3(0, 0, 0));
    root.nodeBox.Max = Ivec3ToUint(ivec3(SVO_MAX_LEVEL_DIM,
        SVO_MAX_LEVEL_DIM, SVO_MAX_LEVEL_DIM));

    uint level, childIndex, nextNodeIndex;
    while( maxT < sceneMaxT )
    {
        // Restart traversal from root.
        curNode = root;
        minT = maxT;
        maxT = sceneMaxT;
        rayEntryPos = rayStartPosSVO + rayDirSVO*minT;

        // Descend until we meet a leaf node.
        bool isLeaf = IsSVOLeafNode(curNode);
        while( !isLeaf )
            //for( int j = 0; j < 2; ++j )
        {
            // Figure out which child node we are in.
            childIndex = GetSVOChildNodeIndex(rayEntryPos, curNode.nodeBox);

            // Locate child node for current tree level.
            uint childrenID = (curNode.info & SVO_NODE_CHILDREN_ID_MASK);
            nextNodeIndex = childrenID*SVO_NODE_TILE_SIZE + childIndex;

            // Update next node to visit.
            curNode = svoNodeBuffer.data[nextNodeIndex];
            isLeaf = IsSVOLeafNode(curNode);
        }

        // Deal with the leaf node.
        if( IsSVONodeFlaged(curNode) )
        {
            hit = true;
            break;
        }

        // Find maxT for the current node.

        vec3 nodeBoxMin = vec3(UintToIvec3(curNode.nodeBox.Min));
        vec3 nodeBoxMax = vec3(UintToIvec3(curNode.nodeBox.Max));
        vec3 minMinusOrigin = nodeBoxMin - rayStartPosSVO;
        vec3 maxMinusOrigin = nodeBoxMax - rayStartPosSVO;
        float t0 = minT, t1 = maxT;

        for( int k = 0; k < 3; ++k )
        {
            float tNear = minMinusOrigin[k] * rayInvDirSVO[k];
            float tFar = maxMinusOrigin[k] * rayInvDirSVO[k];

            if( tNear > tFar )
            {
                // Swap.
                float temp = tNear;
                tNear = tFar;
                tFar = temp;
            }

            t0 = tNear > t0 ? tNear : t0;
            t1 = tFar  < t1 ? tFar : t1;

            if( t0 > t1 )
            {
                return false;
            }
        }

        // Update maxT.
        maxT = t1 + SVO_RAY_T_EPSILON;

    }

    return hit;
}

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

    vec3 indirectColor = vec3(0.0, 0.0, 0.0);
    for( int i = 0; i < sampleVPLCount; ++i )
    {
        VPL vpl = VPLBuffer.vpls[vplBufferIndex + i];

        if( VPLVisibilityTest )
        {
            bool isInShadow = SVOIntersectionTest(PositionWorld.xyz, vpl.WorldPosition.xyz);
            if( isInShadow )
            {
                continue;
            }
        }

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
