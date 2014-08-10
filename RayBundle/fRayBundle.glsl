#version 430 core

struct ListNode
{
	vec4 materialColor;
	vec4 emissionColor;
	vec4 worldPosition;
	vec4 worldNormal;
	uint next;
	float depth;
	bool isLight;
};

varying vec4 vPositionWorld;
varying vec4 vPositionView;
varying vec4 vNormalWorld;

uniform bool isLight;
uniform vec3 materialColor;
uniform vec3 emissionColor;

layout (binding = 0, offset = 0) uniform atomic_uint  gpuMemoryAllocator;
layout (binding = 0, r32ui)      uniform uimage2D     headPointerImage;
layout (std430, binding = 0) buffer gpuMemoryPool
{
	ListNode nodes[];
} rayBundleBuffer;

void main()
{    
    uint newHead = atomicCounterIncrement(gpuMemoryAllocator);
    uint oldHead = imageAtomicExchange(headPointerImage, ivec2(gl_FragCoord.xy), newHead);
    
	vec3 vNormal = normalize(vNormalWorld.xyz);

	ListNode newNode;
	newNode.materialColor.xyz = materialColor;
	newNode.emissionColor.xyz = emissionColor;
	newNode.worldPosition.xyz = vPositionWorld.xyz;
	newNode.worldNormal.xyz = vNormal;
	newNode.next = oldHead;
	newNode.depth = vPositionView.z;
	newNode.isLight = isLight;

	rayBundleBuffer.nodes[newHead] = newNode;

	//vNormal = vNormal*0.5 + 0.5;
	//gl_FragData[0] = vec4(vNormal, 1.0);
}
