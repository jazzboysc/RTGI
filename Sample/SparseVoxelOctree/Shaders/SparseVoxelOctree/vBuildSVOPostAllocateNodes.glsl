#version 430 core

#include "SparseVoxelOctree/sVoxelFragmentList.glsl"
#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    // Update current level node tile range.
    svoNodeBuffer.curLevelStartIndex = svoNodeBuffer.curLevelEndIndex;
    svoNodeBuffer.curLevelEndIndex = atomicCounter(svoNodeAllocator);

    // Update indirect command buffer for SVO allocate nodes pass.
    svoNodeBuffer.allocThreadCountForCurLevel = 
        (svoNodeBuffer.curLevelEndIndex - svoNodeBuffer.curLevelStartIndex)*SVO_NODE_TILE_SIZE;
}