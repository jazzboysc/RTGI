#version 430 core
#extension GL_NV_shader_atomic_float : require

in vec4 gPositionWorld;
in vec4 gNormalWorld;

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

vec4 UintToVec4(uint value)
{
    return vec4(float((value & 0x000000FF)),
                float((value & 0x0000FF00) >> 8U),
                float((value & 0x00FF0000) >> 16U),
                float((value & 0xFF000000) >> 24U));
}

uint Vec4ToUint(vec4 value)
{
    uint res = (uint(value.w) & 0x000000FF) << 24U |
               (uint(value.z) & 0x000000FF) << 16U |
               (uint(value.y) & 0x000000FF) << 8U |
               (uint(value.x) & 0x000000FF);

    return res;
}

void main()
{
    int index = GetIndex(gPositionWorld.xyz);

    vec4 value = vec4(MaterialColor, 1.0);
    value.rgb *= 255.0;
    uint newValue = Vec4ToUint(value);
    uint prevStoredValue = 0;
    uint curStoredValue;

    //while( (curStoredValue = atomicCompSwap(voxelBuffer.data[index].radiance, prevStoredValue, newValue))
    //    != prevStoredValue )
    //{
    //    prevStoredValue = curStoredValue;
    //    vec4 rval = UintToVec4(curStoredValue);
    //    rval.xyz = rval.xyz * rval.w;
    //    vec4 curValF = rval + value;
    //    curValF.xyz /= curValF.w;
    //    newValue = Vec4ToUint(curValF);
    //}

    voxelBuffer.data[index].radiance = newValue;
}
