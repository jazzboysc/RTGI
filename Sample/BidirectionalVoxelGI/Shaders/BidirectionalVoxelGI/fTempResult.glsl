#version 430 core

in vec2 pTCoord;

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

uniform sampler2D tempSampler;
uniform sampler2D tempSampler2;
uniform sampler2DArray tempSamplerArray;

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

void main()
{
    if( ShowMode == 0 )
    {
        gl_FragData[0] = texture(tempSampler, pTCoord);
        
    }
    else if( ShowMode == 1 )
    {
        gl_FragData[0] = texture(tempSamplerArray, vec3(pTCoord, TextureArrayIndex));
    }
    else if( ShowMode == 2 )
    {
        vec4 result = texture(tempSampler, pTCoord) + texture(tempSampler2, pTCoord);

        // Simple gamma tone mapper.
        float greyRadiance = max(0.001, 0.3 * result.r + 0.6 * result.g + 0.1 * result.b);
        float mappedRadiance = pow(min(greyRadiance / 4.5, 1.0), 1.0 / 2.2);
        result.rgb = (mappedRadiance / greyRadiance) * result.rgb;
        result.w = 1.0;

        gl_FragData[0] = result;
    }
    else if( ShowMode == 3 )
    {
        vec4 worldPosition = texture(tempSampler, pTCoord).rgba;
        if( worldPosition.w <= 0.0 )
        {
            discard;
        }

        vec3 normalizedPosition = (worldPosition.xyz - SceneBBMin) / (2.0*SceneBBExtension);
        gl_FragData[0] = vec4(normalizedPosition, 1.0);
    }
    else if( ShowMode == 4 )
    {
        vec4 worldPosition = texture(tempSampler, pTCoord).rgba;
        if( worldPosition.w <= 0.0 )
        {
            discard;
        }

        ivec3 res = GetIndex(worldPosition.xyz);
        int index = res.z * dim * dim + res.y * dim + res.x;
        vec4 color = UintToVec4(voxelBuffer.data[index].value1);
        color.xyz /= 255.0;
        color.w = 1.0;
        
        gl_FragData[0] = color;
    }
}
