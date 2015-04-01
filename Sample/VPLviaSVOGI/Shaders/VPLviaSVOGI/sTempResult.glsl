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

uniform int ShowMode;
uniform int TextureArrayIndex;
uniform vec3 SceneBBMin;
uniform vec3 SceneBBExtension;
uniform int dim;
uniform float positionThreshold;
uniform float normalThreshold;
uniform int kernelSize;

uniform sampler2D tempSampler;
uniform sampler2D tempSampler2;
uniform sampler2DArray tempSamplerArray;
uniform sampler2D positionSampler;
uniform sampler2D normalSampler;

ivec3 GetIndex(vec3 worldPosition)
{
    vec3 imageDim = vec3(float(dim), float(dim), float(dim));
    imageDim = imageDim - vec3(1.0, 1.0, 1.0);

    vec3 offsets = (worldPosition - SceneBBMin) / (2.0*SceneBBExtension);
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