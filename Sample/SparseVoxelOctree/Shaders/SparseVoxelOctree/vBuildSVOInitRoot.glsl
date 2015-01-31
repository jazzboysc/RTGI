#version 430 core

#include "SparseVoxelOctree/sVoxelFragmentList.glsl"
#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    if( voxelFragmentBuffer.count > 0 )
    {
        // Flag root and allocate children nodes for it.
        svoNodeBuffer.rootFlag = 1;
        svoNodeBuffer.rootChild = atomicCounterIncrement(svoNodeAllocator);

        // Update current level node tile range.
        svoNodeBuffer.curLevelStartIndex = svoNodeBuffer.rootChild;
        svoNodeBuffer.curLevelEndIndex = atomicCounter(svoNodeAllocator);

        // Update indirect command buffer for SVO allocate nodes pass.
        svoNodeBuffer.allocThreadCountForCurLevel = 
            (svoNodeBuffer.curLevelEndIndex - svoNodeBuffer.curLevelStartIndex)*SVO_NODE_TILE_SIZE;
        svoNodeBuffer.instanceCount = 1;
        svoNodeBuffer.first = 0;
        svoNodeBuffer.baseInstance = 0;
    }
}