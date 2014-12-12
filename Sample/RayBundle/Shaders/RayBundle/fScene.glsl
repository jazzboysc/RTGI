#version 430 core

const float PI = 3.141592;

varying vec4 vPositionWorld;
varying vec4 vNormalWorld;

uniform bool isLight;

struct AccumulationRadiance
{
	vec4 radiance;
};

layout (std430, binding = 1)  buffer gpuMemoryPool2
{
	AccumulationRadiance data[];
} accumulationBuffer;

ivec3 GetImageCoords(vec3 worldPosition)
{
	// Scene bounding box.
	// x: [-10, 10] y: [0, 20] z: [-10, 10]
	const vec3 minP = vec3(-15.0, 0.0, -15);
	const vec3 maxP = vec3(15.0, 25.0, 15.0);
	vec3 range = maxP - minP;
	vec3 imageDim = vec3(256.0, 256.0, 256.0);

	worldPosition.x = min(max(worldPosition.x, minP.x), maxP.x);
	worldPosition.y = min(max(worldPosition.y, minP.y), maxP.y);
	worldPosition.z = min(max(worldPosition.z, minP.z), maxP.z);
	float x = ((worldPosition.x - minP.x) / range.x) * imageDim.x;
	float y = ((worldPosition.y - minP.y) / range.y) * imageDim.y;
	float z = ((worldPosition.z - minP.z) / range.z) * imageDim.z;

	ivec3 res = ivec3(x, y, z);
	return res;
}

void main()
{
	ivec3 coords = GetImageCoords(vPositionWorld.xyz);
	int index = coords.x + (coords.y + coords.z*256)*256;
    vec4 radiance = accumulationBuffer.data[index].radiance;
	vec3 result = 2.0 * PI * radiance.xyz / radiance.w;

	vec3 normal = normalize(vNormalWorld).xyz;
	normal = normal*0.5 + 0.5;

	// Simple gamma tone mapper.
	float maxRadiance = 24.5;
	float greyRadiance = max(0.001, 0.3 * result.r + 0.6 * result.g + 0.1 * result.b);
	float mappedRadiance = pow(min(greyRadiance / maxRadiance, 1.0), 1.0/2.2);
	result.rgb = (mappedRadiance / greyRadiance) * result.rgb; 

	if( isLight )
	{
		gl_FragData[0] = vec4(vec3(1.0, 1.0, 1.0), 1.0);
	}
	else
	{
		gl_FragData[0] = vec4(result, 1.0);
		//gl_FragData[0] = vec4(normal, 1.0);
	}
}
