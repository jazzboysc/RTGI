#version 430 core

in vec4 vPositionWorld;
in vec4 vNormalWorld;

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
uniform int dim;

ivec3 GetIndex(vec3 worldPosition)
{
    vec3 imageDim = vec3(float(dim), float(dim), float(dim));
    imageDim = imageDim - vec3(1.0, 1.0, 1.0);

    vec3 offsets = (worldPosition - SceneBBCenter + SceneBBExtension) / (2.0*SceneBBExtension);
    offsets = round(offsets*imageDim);
    ivec3 res = ivec3(offsets);
    return res;
}

void main()
{
    ivec3 res = GetIndex(vPositionWorld.xyz);
    int index = res.z * dim * dim + res.y * dim + res.x;
    vec4 color = voxelBuffer.data[index].radiance;

    //vec3 normal = normalize(vNormalWorld).xyz;
    //normal = normal*0.5 + 0.5;
    //float a = float(index) / float(dim*dim*dim - 1);
    //gl_FragData[0] = vec4(a, a, a, 1.0);
    //gl_FragData[0] = vec4(float(float(res.x) / float(dim - 1)), 
    //                      float(float(res.y) / float(dim - 1)), 
    //                      float(float(res.z) / float(dim - 1)), 
    //                      1.0);
    gl_FragData[0] = color;
}
