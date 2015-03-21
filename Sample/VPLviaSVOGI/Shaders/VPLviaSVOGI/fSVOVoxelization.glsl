#version 430 core

in vec4 gPositionWorld;
in vec4 gNormalWorld;

#include "VPLviaSVOGI/sSparseVoxelOctree.glsl"

uniform vec3 MaterialColor;

void main()
{
    // Pack voxel fragment albedo.
    float contrast = length(MaterialColor.rrg - MaterialColor.gbb) / 
        (sqrt(2.0) + MaterialColor.r + MaterialColor.g + MaterialColor.b);
    vec4 value = vec4(MaterialColor, contrast);
    value.rgba *= 255.0;
    uint albedo = Vec4ToUint(value);

    // SVO Voxel Fragment List
    uint newLoc = atomicCounterIncrement(voxelFragmentCounter);
    ivec3 gridPos = WorldToGridPosition(gPositionWorld.xyz);
    VoxelFragment voxelFragment;
    voxelFragment.albedo = albedo;
    voxelFragment.gridPosition = Ivec3ToUint(gridPos);
    voxelFragment.value1 = 0;
    voxelFragment.value2 = 0;
    voxelFragmentBuffer.data[newLoc] = voxelFragment;
}
