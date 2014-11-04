#version 430 core

in vec4 gPositionWorld;
in vec4 gNormalWorld;

struct Voxel
{
    vec4 radiance;
};

layout(std430, binding = 0)  buffer gpuMemoryPool
{
    Voxel data[];
} voxelBuffer;

uniform vec3 SceneBBCenter;
uniform vec3 SceneBBExtension;
uniform vec3 MaterialColor;
uniform int dim;

int GetIndex(vec3 worldPosition)
{
    vec3 imageDim = vec3(float(dim), float(dim), float(dim));
    imageDim = imageDim - vec3(1.0, 1.0, 1.0);

    vec3 offsets = (worldPosition - SceneBBCenter + SceneBBExtension) / (2.0*SceneBBExtension);
    offsets = round(offsets*imageDim);
    //offsets = (offsets*imageDim);

    ivec3 res = ivec3(offsets);
    int index = res.z * dim * dim + res.y * dim + res.x;
    return index;
}

void main()
{
	vec3 normal = normalize(gNormalWorld).xyz;
	normal = normal*0.5 + 0.5;

    int index = GetIndex(gPositionWorld.xyz);
    
    voxelBuffer.data[index].radiance = vec4(MaterialColor, 0.0);
}
