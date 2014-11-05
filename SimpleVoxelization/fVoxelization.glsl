#version 430 core

in vec4 gPositionWorld;
in vec4 gNormalWorld;

struct Voxel
{
    vec4 radiance;
    vec4 radiance2;
    vec4 radiance3;
};

layout(std430, binding = 0)  buffer gpuMemoryPool
{
    Voxel data[];
} voxelBuffer;

uniform vec3 SceneBBCenter;
uniform vec3 SceneBBExtension;
uniform vec3 Inv2SceneBBExtension;
uniform vec3 MaterialColor;
uniform int dim;

int GetIndex(vec3 worldPosition)
{
    vec3 imageDim = vec3(float(dim), float(dim), float(dim));
    imageDim = imageDim - vec3(1.0, 1.0, 1.0);

    vec3 offsets = (worldPosition - SceneBBCenter + SceneBBExtension) * Inv2SceneBBExtension;
    offsets = offsets*imageDim + vec3(0.5, 0.5, 0.5);
    ivec3 res = ivec3(offsets);
    int index = res.z * dim * dim + res.y * dim + res.x;
    return index;
}

void main()
{
    int index = GetIndex(gPositionWorld.xyz);
    voxelBuffer.data[index].radiance = vec4(MaterialColor, 0.0);
    voxelBuffer.data[index].radiance2 = vec4(MaterialColor, 1.0);
    voxelBuffer.data[index].radiance3 = vec4(MaterialColor, 2.0);
}
