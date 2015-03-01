#version 430 core

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    if( voxelFragmentBuffer.count > 0 )
    {
        // Flag root and allocate children nodes for it.
        svoCommandBuffer.rootFlag = 1234;
        svoCommandBuffer.rootChild = atomicCounterIncrement(svoNodeAllocator);

        // Update current level node tile range.
        svoCommandBuffer.curLevelStartIndex = svoCommandBuffer.rootChild;
        svoCommandBuffer.curLevelEndIndex = atomicCounter(svoNodeAllocator);

        // Update indirect command buffer for SVO allocate nodes pass.
        svoCommandBuffer.allocThreadCountForCurLevel =
            (svoCommandBuffer.curLevelEndIndex - svoCommandBuffer.curLevelStartIndex)*SVO_NODE_TILE_SIZE;
        svoCommandBuffer.instanceCount = 1;
        svoCommandBuffer.first = 0;
        svoCommandBuffer.baseInstance = 0;

        // Create SVO root node bound.
        SVONodeAABB nodeBox;
        nodeBox.Min = Ivec3ToUint(ivec3(0, 0, 0));
        nodeBox.Max = Ivec3ToUint(ivec3(svoUniformBuffer.dim, svoUniformBuffer.dim, 
            svoUniformBuffer.dim));

        // Create first level node boxes.
        uint childID;
        for( uint i = 0; i < SVO_NODE_TILE_SIZE; ++i )
        {
            childID = svoCommandBuffer.rootChild*SVO_NODE_TILE_SIZE + i;
            SVONodeAABB childNodeBox = GetSVOChildNodeBox(i, nodeBox);
            uvec4 nodeData;
            nodeData.x = SVO_NODE_LEAF_MASK;
            nodeData.y = 0;
            nodeData.z = childNodeBox.Max;
            nodeData.w = childNodeBox.Min;
            imageStore(svoNodeBuffer, int(childID), nodeData);
        }
    }
    else
    {
        svoCommandBuffer.rootFlag = 0;
    }
}