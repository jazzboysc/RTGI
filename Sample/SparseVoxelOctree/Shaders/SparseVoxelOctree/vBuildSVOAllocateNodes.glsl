#version 430 core

#include "SparseVoxelOctree/sVoxelFragmentList.glsl"
#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    uint nodeIndexToCheck = svoNodeBuffer.curLevelStartIndex*SVO_NODE_TILE_SIZE + gl_VertexID;
    if( svoNodeBuffer.data[nodeIndexToCheck].flag == SVO_NODE_FLAGED )
    {
        svoNodeBuffer.data[nodeIndexToCheck].child = atomicCounterIncrement(svoNodeAllocator);
    }
}