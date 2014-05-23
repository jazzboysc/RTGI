#version 150
#extension GL_EXT_gpu_shader4 : enable

in vec2 pTCoord;

uniform sampler2D ssdoSampler;
uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D colorSampler;
uniform sampler2D directLightingSampler;

uniform float positionThreshold;
uniform float normalThreshold;
uniform float maxRadiance;
uniform int kernelSize;

void main()
{
 	vec3 position = texture(positionSampler, pTCoord).rgb;
 	vec3 normal = texture(normalSampler, pTCoord).rgb;
	normal = normal*2.0 - 1.0;
 	vec3 color = texture(colorSampler, pTCoord).rgb;

	vec2 textureSize = textureSize2D(positionSampler, 0);
	vec2 invTextureSize = 1.0 / textureSize;
 	
	// Skip undefined pixels.
 	if( length(normal) == 0.0 )
	{
 		gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
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
 			vec3 sampleRadiance = texture(ssdoSampler, sampleTexCoord).rgb;
 			vec3 samplePosition = texture(positionSampler, sampleTexCoord).rgb;
 			vec3 sampleNormal = texture(normalSampler, sampleTexCoord).rgb;
 			
 			if(	(length(samplePosition - position) < positionThreshold) && 
 				(dot(sampleNormal, normal) > (1.0 - normalThreshold)))
 			{
 				float weight = 1.0;
 				weightedRadiance += weight * sampleRadiance;				
 				weightSum += weight;
 			}

			radiance += sampleRadiance;
 		}
 	}

 	// Multiply by surface reflectance.
	radiance *= color;
 	weightedRadiance *= color;
 	
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

	// Add direct light and indirect light.
	vec3 directLight = texture(directLightingSampler, pTCoord).rgb;
	result.rgb = 1.0 * result.rgb + 0.5 * directLight;

	// Simple gamma tone mapper.
	float greyRadiance = max(0.001, 0.3 * result.r + 0.6 * result.g + 0.1 * result.b);
	float mappedRadiance = pow(min(greyRadiance / maxRadiance, 1.0), 1.0/2.2);
	result.rgb = (mappedRadiance / greyRadiance) * result.rgb; 

	result.a = 1.0;
	gl_FragColor = result;
}
