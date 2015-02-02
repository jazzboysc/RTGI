#version 430 core

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    uint curNodeID = svoNodeBuffer.curLevelStartIndex*SVO_NODE_TILE_SIZE + gl_VertexID;
    if( IsSVONodeFlaged(curNodeID) )
    {
        // Allocate children tile.
        svoNodeBuffer.data[curNodeID].child = atomicCounterIncrement(svoNodeAllocator);

        // Create next level node boxes for the children tile.
        uint childrenStartIndex = svoNodeBuffer.data[curNodeID].child * SVO_NODE_TILE_SIZE;
        for( uint i = 0; i < SVO_NODE_TILE_SIZE; ++i )
        {
            svoNodeBuffer.data[childrenStartIndex + i].nodeBox = 
                GetSVOChildNodeBox(i, svoNodeBuffer.data[curNodeID].nodeBox);
        }
    }
}