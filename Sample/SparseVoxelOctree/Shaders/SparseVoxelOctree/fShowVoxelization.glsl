#version 430 core

in vec4 vPositionWorld;
in vec4 vNormalWorld;

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

uniform vec3 SceneBBCenter;
uniform vec3 SceneBBExtension;
uniform int dim;
uniform int ShowWorldPosition;

ivec3 GetVoxelGridPosition(vec3 worldPosition)
{
    vec3 imageDim = vec3(float(dim), float(dim), float(dim));
    imageDim = imageDim - vec3(1.0, 1.0, 1.0);

    vec3 offsets = (worldPosition - SceneBBCenter + SceneBBExtension) / (2.0*SceneBBExtension);
    offsets = round(offsets*imageDim);
    ivec3 res = ivec3(offsets);
    return res;
}

vec4 UintToVec4(uint value)
{
    return vec4(float((value & 0x000000FF)),
        float((value & 0x0000FF00) >> 8U),
        float((value & 0x00FF0000) >> 16U),
        float((value & 0xFF000000) >> 24U));
}

vec4 FetchColorFromSVO(ivec3 gridPosition)
{
    vec4 color;

    // Create SVO root node bound.
    SVONodeAABB nodeBox;
    nodeBox.Min = Ivec3ToUint(ivec3(0, 0, 0));
    nodeBox.Max = Ivec3ToUint(ivec3(svoUniformBuffer.dim, svoUniformBuffer.dim, svoUniformBuffer.dim));

    uint childIndex;
    uint nodeTileIndex = 0;
    uint nextNodeIndex;
    for( int i = 1; i <= svoUniformBuffer.curLevel + 1; ++i )
    {
        // Figure out which child node to visit.
        childIndex = GetSVOChildNodeIndex(gridPosition, nodeBox);

        // Locate child node for current tree level.
        nextNodeIndex = nodeTileIndex*SVO_NODE_TILE_SIZE + childIndex;

        if( !IsSVONodeFlaged(nextNodeIndex) )
        {
            return vec4(0.0, 0.0, 0.0, 1.0);
        }

        // Update node tile index to visit.
        nodeTileIndex = svoNodeBuffer.data[nextNodeIndex].child;

        // Update node AABB to visit.
        nodeBox = svoNodeBuffer.data[nextNodeIndex].nodeBox;
    }

    if( !IsSVONodeFlaged(nextNodeIndex) )
    {
        return vec4(0.0, 0.0, 0.0, 1.0);
    }

    color = UintToVec4(svoNodeBuffer.data[nextNodeIndex].albedo);
    color.xyz /= 255.0;
    color.w = 1.0;

    return color;
}

void main()
{
    ivec3 gridPos = GetVoxelGridPosition(vPositionWorld.xyz);

    if( ShowWorldPosition == 1 )
    {
        gl_FragData[0] = vec4(float(float(gridPos.x) / float(dim - 1)),
            float(float(gridPos.y) / float(dim - 1)),
            float(float(gridPos.z) / float(dim - 1)),
            1.0);
    }
    else
    {
        vec4 color = FetchColorFromSVO(gridPos);
        gl_FragData[0] = color;
    }
}
