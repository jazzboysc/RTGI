#version 430 core

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    uint nodeIndexToCheck = svoCommandBuffer.curLevelStartIndex*SVO_NODE_TILE_SIZE + gl_VertexID;
    uvec4 nodeData = imageLoad(svoNodeBuffer, int(nodeIndexToCheck));
    nodeData.x = SVO_NODE_LEAF_MASK;
    nodeData.y = 0;
    imageStore(svoNodeBuffer, int(nodeIndexToCheck), nodeData);
}