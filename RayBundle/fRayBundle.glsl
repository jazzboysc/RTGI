#version 430 core

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

varying vec4 vPositionWorld;
varying vec4 vNormalWorld;
varying vec4 vPositionView;

uniform bool isLight;
uniform vec3 materialColor;
uniform vec3 emissionColor;

// Turn on early fragment testing.
layout (early_fragment_tests) in;

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
	newNode.next = oldHead;
	newNode.depth = vPositionView.z;
	newNode.isLight = isLight;
	newNode.materialColor = materialColor;
	newNode.emissionColor = emissionColor;
	newNode.worldPosition = vPositionWorld.xyz;
	newNode.worldNormal = vNormal;
	rayBundleBuffer.nodes[newHead] = newNode;

	//if( !isLight )
	//{
	//	gl_FragData[0] = vec4(materialColor, 0.0);
	//}
	//else
	//{
	//	gl_FragData[0] = vec4(emissionColor, 0.0);
	//}
}
