#version 430 core

#include "SparseVoxelOctree/sVoxelFragmentList.glsl"
#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

void main()
{
    if( voxelFragmentBuffer.count > 0 )
    {
        svoNodeBuffer.rootChild = atomicCounterIncrement(svoNodeAllocator);
        svoNodeBuffer.rootFlag = 1;
    }
}