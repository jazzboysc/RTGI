#version 430 core
#extension GL_EXT_gpu_shader4 : enable  // for textureSize2D function.

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

uniform float positionThreshold;
uniform float normalThreshold;
uniform float maxRadiance;
uniform int kernelSize;

uniform sampler2D tempSampler;
uniform sampler2D tempSampler2;
uniform sampler2DArray tempSamplerArray;
uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D colorSampler;

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
        vec3 position = texture2D(positionSampler, pTCoord).rgb;
        vec3 normal = texture2D(normalSampler, pTCoord).rgb;
        normal = normal*2.0 - 1.0;
        vec3 color = texture2D(colorSampler, pTCoord).rgb;

        vec2 textureSize = textureSize2D(positionSampler, 0);
        vec2 invTextureSize = 1.0 / textureSize;

        // Skip undefined pixels.
        if( length(normal) == 0.0 )
        {
            gl_FragData[0] = vec4(0.0);
            return;
        }

        vec3 weightedRadiance = vec3(0);
        vec3 radiance = vec3(0);
        float weightSum = 0.0;

        for( int i = -kernelSize; i <= kernelSize; i++ )
        {

            for( int j = -kernelSize; j <= kernelSize; j++ )
            {

                vec2 sampleTexCoord = pTCoord + vec2(j, i) * invTextureSize;
                vec3 sampleRadiance = texture2D(tempSampler, sampleTexCoord).rgb;
                vec3 samplePosition = texture2D(positionSampler, sampleTexCoord).rgb;
                vec3 sampleNormal = texture2D(normalSampler, sampleTexCoord).rgb;

                if( (length(samplePosition - position) < positionThreshold) &&
                    (dot(sampleNormal, normal) > (1.0 - normalThreshold)) )
                {
                    float weight = 1.0;
                    weightedRadiance += weight * sampleRadiance;
                    weightSum += weight;
                }

                radiance += sampleRadiance;
            }
        }

        vec4 result;
        if( weightSum > 0.0 )
        {
            weightedRadiance /= weightSum;
            result.rgb = weightedRadiance.rgb;
        }
        else
        {
            result.rgb = radiance.rgb / float((2 * kernelSize + 1) * (2 * kernelSize + 1));    // simple average if weightSum == 0
        }

        result += texture(tempSampler2, pTCoord);

        // Simple gamma tone mapper.
        float greyRadiance = max(0.001, 0.3 * result.r + 0.6 * result.g + 0.1 * result.b);
        float mappedRadiance = pow(min(greyRadiance / 4.5, 1.0), 1.0 / 2.2);
        result.rgb = (mappedRadiance / greyRadiance) * result.rgb;
        result.a = 1.0;

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
    else if( ShowMode == 5 )
    {
        vec3 position = texture2D(positionSampler, pTCoord).rgb;
        vec3 normal = texture2D(normalSampler, pTCoord).rgb;
        normal = normal*2.0 - 1.0;
        vec3 color = texture2D(colorSampler, pTCoord).rgb;

        vec2 textureSize = textureSize2D(positionSampler, 0);
        vec2 invTextureSize = 1.0 / textureSize;

        // Skip undefined pixels.
        if( length(normal) == 0.0 )
        {
            gl_FragData[0] = vec4(0.0);
            return;
        }

        vec3 weightedRadiance = vec3(0);
        vec3 radiance = vec3(0);
        float weightSum = 0.0;

        for( int i = -kernelSize; i <= kernelSize; i++ )
        {

            for( int j = -kernelSize; j <= kernelSize; j++ )
            {

                vec2 sampleTexCoord = pTCoord + vec2(j, i) * invTextureSize;
                vec3 sampleRadiance = texture2D(tempSampler, sampleTexCoord).rgb;
                vec3 samplePosition = texture2D(positionSampler, sampleTexCoord).rgb;
                vec3 sampleNormal = texture2D(normalSampler, sampleTexCoord).rgb;

                if( (length(samplePosition - position) < positionThreshold) &&
                    (dot(sampleNormal, normal) > (1.0 - normalThreshold)) )
                {
                    float weight = 1.0;
                    weightedRadiance += weight * sampleRadiance;
                    weightSum += weight;
                }

                radiance += sampleRadiance;
            }
        }

        vec4 result;
        if( weightSum > 0.0 )
        {
            weightedRadiance /= weightSum;
            result.rgb = weightedRadiance.rgb;
        }
        else
        {
            result.rgb = radiance.rgb / float((2 * kernelSize + 1) * (2 * kernelSize + 1));    // simple average if weightSum == 0
        }

        gl_FragData[0] = result;
    }
}
