#version 430 core

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    // Update current level node tile range.
    svoCommandBuffer.curLevelStartIndex = svoCommandBuffer.curLevelEndIndex;
    svoCommandBuffer.curLevelEndIndex = atomicCounter(svoNodeAllocator);

    // Update indirect command buffer for SVO allocate nodes pass.
    svoCommandBuffer.allocThreadCountForCurLevel =
        (svoCommandBuffer.curLevelEndIndex - svoCommandBuffer.curLevelStartIndex)*SVO_NODE_TILE_SIZE;

    // Update indirect command buffer for SVO visualization pass.
    svoCommandBuffer.ic2PrimCount = svoCommandBuffer.curLevelEndIndex*SVO_NODE_TILE_SIZE;
}