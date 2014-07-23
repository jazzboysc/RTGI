#version 420 core

varying vec4 vNormalView;

// Turn on early fragment testing.
layout (early_fragment_tests) in;

// Atomic counter.
layout (binding = 0, offset = 0) uniform atomic_uint gpuMemoryAllocator;

// Linked lists GPU memory pool.
layout (binding = 0, rgba32ui) uniform imageBuffer gpuMemoryPool;

// Head pointer 2D image buffer.
layout (binding = 1, r32ui) uniform imageRect headPointerImage;

void main()
{
	vec3 vNormal = normalize(vNormalView.xyz);
	vNormal.xyz = (vNormal.xyz + 1.0) * 0.5;
    vec4 newNodeData = vec4(vNormal, 0.0);
    
    uint newHead = atomicCounterIncrement(gpuMemoryAllocator);
    
    uint oldHead = imageAtomicExchange(headPointerImage, ivec2(gl_FragCoord.xy), newHead);
    
    uvec4 newNode;
    newNode.x = oldHead;
    newNode.y = packUnorm4x8(newNodeData);
    newNode.z = floatBitsToUint(gl_FragCoord.z);
    newNode.w = 0;
    
    imageStore(gpuMemoryPool, newHead, newNode);

	gl_FragData[0] = vec4(vNormal, 0.0);
}
