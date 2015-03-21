#version 430 core

#include "VPLviaSVOGI/sSparseVoxelOctree.glsl"

void main()
{
    if( voxelFragmentBuffer.count > 0 )
    {
        // Flag root and allocate children nodes for it.
        svoNodeBuffer.rootFlag = 1234;
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

        // Create SVO root node bound.
        SVONodeAABB nodeBox;
        nodeBox.Min = Ivec3ToUint(ivec3(0, 0, 0));
        nodeBox.Max = Ivec3ToUint(ivec3(svoUniformBuffer.dim, svoUniformBuffer.dim, 
            svoUniformBuffer.dim));

        // Create first level node boxes.
        uint childID;
        for( uint i = 0; i < SVO_NODE_TILE_SIZE; ++i )
        {
            childID = svoNodeBuffer.rootChild*SVO_NODE_TILE_SIZE + i;
            svoNodeBuffer.data[childID].nodeBox = GetSVOChildNodeBox(i, nodeBox);
            InitSVONode(childID);
        }
    }
    else
    {
        svoNodeBuffer.rootFlag = 0;
    }
}