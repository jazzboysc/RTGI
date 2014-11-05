#version 430 core

in vec4 vPositionWorld;
in vec4 vNormalWorld;

struct Voxel
{
    uint radiance;
    //vec4 radiance2;
    //vec4 radiance3;
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

vec4 UintToVec4(uint value)
{
    return vec4(float((value & 0x000000FF)),
        float((value & 0x0000FF00) >> 8U),
        float((value & 0x00FF0000) >> 16U),
        float((value & 0xFF000000) >> 24U));
}

void main()
{
    ivec3 res = GetIndex(vPositionWorld.xyz);
    int index = res.z * dim * dim + res.y * dim + res.x;
    vec4 color = UintToVec4(voxelBuffer.data[index].radiance);
    color.xyz /= 255.0;
    color.w = 1.0;

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
