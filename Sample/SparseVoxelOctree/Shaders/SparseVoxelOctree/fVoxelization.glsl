#version 430 core

in vec4 gPositionWorld;
in vec4 gNormalWorld;

//----------------------------------------------------------------------------
// SVO Voxel Fragment List
//----------------------------------------------------------------------------
layout(binding = 0, offset = 0) uniform atomic_uint voxelFragmentCounter;

struct VoxelFragment
{
    uint gridPosition;
    uint albedo;
    uint value1;
    uint value2;
};

layout(std430, binding = 1)  buffer _voxelFragmentBuffer
{
    uint  count;
    uint  instanceCount;
    uint  first;
    uint  baseInstance;

    VoxelFragment data[]; // must be big enough to hold all voxel fragments.
} voxelFragmentBuffer;
//----------------------------------------------------------------------------
uint Ivec3ToUint(ivec3 value)
{
    uint res = (uint(value.z) & 0x000003FF) << 20U |
               (uint(value.y) & 0x000003FF) << 10U |
               (uint(value.x) & 0x000003FF);

    return res;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
struct Voxel
{
    uint value1;
    uint value2;
    uint value3;
    uint value4;
};

layout(std430, binding = 0)  buffer _voxelBuffer
{
    Voxel data[];
} voxelBuffer;

uniform vec3 SceneBBCenter;
uniform vec3 SceneBBExtension;
uniform vec3 Inv2SceneBBExtension;
uniform vec3 MaterialColor;
uniform int dim;

ivec3 GetGridPosition(vec3 worldPosition)
{
    vec3 imageDim = vec3(float(dim), float(dim), float(dim));
    imageDim = imageDim - vec3(1.0, 1.0, 1.0);

    vec3 offsets = (worldPosition - SceneBBCenter + SceneBBExtension) * Inv2SceneBBExtension;
    offsets = offsets*imageDim + vec3(0.5, 0.5, 0.5);
    ivec3 res = ivec3(offsets);

    return res;
}

int GetIndex(vec3 worldPosition)
{
    ivec3 res = GetGridPosition(worldPosition);
    int index = res.z * dim * dim + res.y * dim + res.x;
    return index;
}

uint Vec4ToUint(vec4 value)
{
    uint res = (uint(value.w) & 0x000000FF) << 24U |
               (uint(value.z) & 0x000000FF) << 16U |
               (uint(value.y) & 0x000000FF) << 8U |
               (uint(value.x) & 0x000000FF);

    return res;
}
//----------------------------------------------------------------------------

void main()
{
    int index = GetIndex(gPositionWorld.xyz);

    float contrast = length(MaterialColor.rrg - MaterialColor.gbb) / 
        (sqrt(2.0) + MaterialColor.r + MaterialColor.g + MaterialColor.b);
    vec4 value = vec4(MaterialColor, contrast);
    value.rgba *= 255.0;

    uint newValue = Vec4ToUint(value);
    atomicMax(voxelBuffer.data[index].value1, newValue);
    voxelBuffer.data[index].value2 = 1;

    // SVO Voxel Fragment List
    uint newLoc = atomicCounterIncrement(voxelFragmentCounter);
    ivec3 gridPos = GetGridPosition(gPositionWorld.xyz);
    VoxelFragment voxelFragment;
    voxelFragment.albedo = newValue;
    voxelFragment.gridPosition = Ivec3ToUint(gridPos);
    voxelFragment.value1 = 0;
    voxelFragment.value2 = 0;
    voxelFragmentBuffer.data[newLoc] = voxelFragment;
}
