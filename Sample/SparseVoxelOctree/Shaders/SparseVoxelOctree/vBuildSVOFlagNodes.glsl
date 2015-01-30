#version 430 core

#include "SparseVoxelOctree/sVoxelFragmentList.glsl"
#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

uniform uint curLevel;

void main()
{
    VoxelFragment voxelFragment = voxelFragmentBuffer.data[gl_VertexID];
    ivec3 voxelGridPos = UintToIvec3(voxelFragment.gridPosition);
    svoNodeBuffer.data[gl_VertexID].flag = gl_VertexID;
}