#version 430 core

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    uint curNodeID = svoCommandBuffer.curLevelStartIndex*SVO_NODE_TILE_SIZE + gl_VertexID;
    if( IsSVONodeFlaged(curNodeID) )
    {
        // Allocate children tile.
        uint childrenID = atomicCounterIncrement(svoNodeAllocator);
        uvec4 nodeData = imageLoad(svoNodeBuffer, int(curNodeID));
        nodeData.x |= childrenID;
        nodeData.x &= ~SVO_NODE_LEAF_MASK;
        imageStore(svoNodeBuffer, int(curNodeID), nodeData);

        // Create next level node boxes for the children tile.
        SVONodeAABB nodeBox;
        nodeBox.Max = nodeData.z;
        nodeBox.Min = nodeData.w;
        uint childrenStartIndex = (nodeData.x & SVO_NODE_CHILDREN_ID_MASK) * SVO_NODE_TILE_SIZE;
        for( uint i = 0; i < SVO_NODE_TILE_SIZE; ++i )
        {
            SVONodeAABB childNodeBox = GetSVOChildNodeBox(i, nodeBox);
            uvec4 childNodeData = imageLoad(svoNodeBuffer, int(childrenStartIndex + i));
            childNodeData.z = childNodeBox.Max;
            childNodeData.w = childNodeBox.Min;
            imageStore(svoNodeBuffer, int(childrenStartIndex + i), childNodeData);
        }
    }
}