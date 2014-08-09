#version 430 core

varying vec4 vPositionWorld;
varying vec4 vNormalWorld;

struct ListNode
{
	uint next;
	float depth;
	bool isLight;
	vec3 materialColor;
	vec3 emissionColor;
	vec3 worldPosition;
	vec3 worldNormal;
};

layout (std430, binding = 0)  buffer gpuMemoryPool
{
	ListNode nodes[];
} rayBundleBuffer;

struct AccumulationRadiance
{
	vec3 radiance;
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
    vec3 radiance = accumulationBuffer.data[index].radiance;

	vec3 normal = normalize(vNormalWorld).xyz;
	normal = normal*0.5 + 0.5;

	//gl_FragData[0] = vec4(radiance, 1.0);
	gl_FragData[0] = vec4(normal, 1.0);
}
