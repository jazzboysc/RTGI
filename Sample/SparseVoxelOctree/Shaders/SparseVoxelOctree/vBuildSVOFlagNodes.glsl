#version 430 core

#include "SparseVoxelOctree/sVoxelFragmentList.glsl"
#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    // Fetch voxel fragment for the thread.
    VoxelFragment voxelFrag = voxelFragmentBuffer.data[gl_VertexID];

    // Create SVO root node bound.
    SVONodeAABB nodeBox;
    nodeBox.Min = ivec3(0, 0, 0);
    nodeBox.Max = ivec3(svoUniformBuffer.dim, svoUniformBuffer.dim, svoUniformBuffer.dim);

    ivec3 voxelGridPos = UintToIvec3(voxelFrag.gridPosition);
    uint childIndex;
    uint nodeTileIndex = 0;
    uint nextNodeIndex;
    for( int i = 1; i <= svoUniformBuffer.curLevel; ++i )
    {
        childIndex = GetSVOChildNodeIndex(voxelGridPos, nodeBox);
        
        // Locate SVO nodes for current tree level.
        nextNodeIndex = nodeTileIndex*SVO_NODE_TILE_SIZE + childIndex;

        // Update node tile index to visit.
        nodeTileIndex = svoNodeBuffer.data[nextNodeIndex].child;

        // Update node AABB to visit.
        nodeBox = GetSVOChildNodeBox(childIndex, nodeBox);
    }
    svoNodeBuffer.data[nextNodeIndex].flag = SVO_NODE_FLAGED;
}