#version 430 core

#include "VPLviaSVOGI/sSparseVoxelOctree.glsl"

void main()
{
    uint nodeIndexToCheck = svoNodeBuffer.curLevelStartIndex*SVO_NODE_TILE_SIZE + gl_VertexID;
    InitSVONode(nodeIndexToCheck);
}