#version 430 core

#include "BidirectionalVoxelGI/sSparseVoxelOctree.glsl"

void main()
{
    // Fetch voxel fragment for the thread.
    VoxelFragment voxelFrag = voxelFragmentBuffer.data[gl_VertexID];

    // Create SVO root node bound.
    SVONodeAABB nodeBox;
    nodeBox.Min = Ivec3ToUint(ivec3(0, 0, 0));
    nodeBox.Max = Ivec3ToUint(ivec3(svoUniformBuffer.dim, svoUniformBuffer.dim, svoUniformBuffer.dim));

    ivec3 voxelGridPos = UintToIvec3(voxelFrag.gridPosition);
    uint childIndex;
    uint nodeTileIndex = 0;
    uint nextNodeIndex;
    for( int i = 1; i <= svoUniformBuffer.curLevel; ++i )
    {
        // Figure out which child node to visit.
        childIndex = GetSVOChildNodeIndex(voxelGridPos, nodeBox);
        
        // Locate child node for current tree level.
        nextNodeIndex = nodeTileIndex*SVO_NODE_TILE_SIZE + childIndex;

        // Update node tile index to visit.
        nodeTileIndex = (svoNodeBuffer.data[nextNodeIndex].info & SVO_NODE_CHILDREN_ID_MASK);

        // Update node AABB to visit.
        nodeBox = svoNodeBuffer.data[nextNodeIndex].nodeBox;
    }
    
    FlagSVONode(nextNodeIndex);
}