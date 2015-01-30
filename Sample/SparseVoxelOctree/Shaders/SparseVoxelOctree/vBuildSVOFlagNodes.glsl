#version 430 core

#include "SparseVoxelOctree/sVoxelFragmentList.glsl"
#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

uniform uint curLevel;
uniform uint dim;

void main()
{
    VoxelFragment voxelFrag = voxelFragmentBuffer.data[gl_VertexID];

    SVONodeAABB nodeBox;
    nodeBox.Min = ivec3(0, 0, 0);
    nodeBox.Max = ivec3(128, 128, 128);

    ivec3 voxelGridPos;
    uint childIndex;
    for( int i = 0; i < 8; ++i )
    {
        voxelGridPos = UintToIvec3(voxelFrag.gridPosition);
        childIndex = GetSVOChildNodeIndex(voxelGridPos, nodeBox);
        childIndex++;
    }

    svoNodeBuffer.data[gl_VertexID].child = childIndex;
    svoNodeBuffer.data[gl_VertexID].flag = gl_VertexID;
}