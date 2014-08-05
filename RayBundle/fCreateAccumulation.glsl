#version 430 core

varying vec4 vPositionView;

uniform vec3 materialColor;
uniform vec3 emissionColor;

// Turn on early fragment testing.
layout (early_fragment_tests) in;

layout (binding = 0, offset = 0) uniform atomic_uint  gpuMemoryAllocator;
layout (binding = 0, r32ui)      uniform uimage2D     headPointerImage;

struct ListNode
{
	uint next;
	float depth;
	vec3 illumination;
};

layout (std430, binding = 0) buffer gpuMemoryPool
{
	ListNode nodes[];
} accumulationBuffer;

void main()
{    
    uint newHead = atomicCounterIncrement(gpuMemoryAllocator);
    uint oldHead = imageAtomicExchange(headPointerImage, ivec2(gl_FragCoord.xy), newHead);

	ListNode newNode;
	newNode.next = oldHead;
	newNode.depth = vPositionView.z;
    newNode.illumination = vec3(0.0, 0.0, 0.0);

	accumulationBuffer.nodes[newHead] = newNode;

	gl_FragData[0] = vec4(materialColor, 0.0);
}
