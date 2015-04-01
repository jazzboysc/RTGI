#version 430 core
#extension GL_EXT_gpu_shader4 : enable  // for textureSize2D function.

#include "VPLviaSVOGI/sTempResult.glsl"

in vec2 pTCoord;
out vec4 Output;

uniform sampler2D tempSampler;
uniform sampler2D positionSampler;
uniform sampler2D normalSampler;

uniform float positionThreshold;
uniform float normalThreshold;
uniform int kernelSize;

void main()
{
    vec3 position = texture2D(positionSampler, pTCoord).rgb;
    vec3 normal = texture2D(normalSampler, pTCoord).rgb;
    normal = normal*2.0 - 1.0;

    vec2 textureSize = textureSize2D(positionSampler, 0);
    vec2 invTextureSize = 1.0 / textureSize;

    // Skip undefined pixels.
    if( length(normal) == 0.0 )
    {
        Output = vec4(0.0);
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

    Output = result;
}