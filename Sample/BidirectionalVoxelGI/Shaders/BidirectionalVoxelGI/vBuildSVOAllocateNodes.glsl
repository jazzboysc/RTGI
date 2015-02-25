#version 430 core

#include "BidirectionalVoxelGI/sSparseVoxelOctree.glsl"

void main()
{
    uint curNodeID = svoNodeBuffer.curLevelStartIndex*SVO_NODE_TILE_SIZE + gl_VertexID;
    if( IsSVONodeFlaged(curNodeID) )
    {
        // Allocate children tile.
        uint childrenID = atomicCounterIncrement(svoNodeAllocator);
        svoNodeBuffer.data[curNodeID].info |= childrenID;
        svoNodeBuffer.data[curNodeID].info &= ~SVO_NODE_LEAF_MASK;

        // Create next level node boxes for the children tile.
        uint childrenStartIndex = (svoNodeBuffer.data[curNodeID].info & SVO_NODE_CHILDREN_ID_MASK) * SVO_NODE_TILE_SIZE;
        for( uint i = 0; i < SVO_NODE_TILE_SIZE; ++i )
        {
            svoNodeBuffer.data[childrenStartIndex + i].nodeBox = 
                GetSVOChildNodeBox(i, svoNodeBuffer.data[curNodeID].nodeBox);
        }
    }
}