#version 420 core

varying vec4 vNormalView;

// Turn on early fragment testing.
layout (early_fragment_tests) in;

layout (binding = 0, offset = 0) uniform atomic_uint  gpuMemoryAllocator;
layout (binding = 0, r32ui)      uniform uimage2D     headPointerImage;
layout (binding = 1, rgba32ui)   uniform uimageBuffer gpuMemoryPool;

void main()
{
	vec3 vNormal = normalize(vNormalView.xyz);
	vNormal.xyz = (vNormal.xyz + 1.0) * 0.5;
    vec4 newNodeData = vec4(vNormal, 0.5);
    
    uint newHead = atomicCounterIncrement(gpuMemoryAllocator);
    
    uint oldHead = imageAtomicExchange(headPointerImage, ivec2(gl_FragCoord.xy), newHead);
    
    uvec4 newNode;
    newNode.x = oldHead;
    newNode.y = packUnorm4x8(newNodeData);
    newNode.z = floatBitsToUint(gl_FragCoord.z);
    newNode.w = 0;
    
    imageStore(gpuMemoryPool, int(newHead), newNode);

	gl_FragData[0] = vec4(vNormal, 0.0);
}
