#version 420 core

varying vec4 vPositionView;

uniform vec3 materialColor;
uniform vec3 emissionColor;

// Turn on early fragment testing.
layout (early_fragment_tests) in;

layout (binding = 0, offset = 0) uniform atomic_uint  gpuMemoryAllocator;
layout (binding = 0, r32ui)      uniform uimage2D     headPointerImage;
layout (binding = 1, rgba32ui)   uniform uimageBuffer gpuMemoryPool;

void main()
{    
    uint newHead = atomicCounterIncrement(gpuMemoryAllocator);
    uint oldHead = imageAtomicExchange(headPointerImage, ivec2(gl_FragCoord.xy), newHead);
    
    uvec4 newNode;
    newNode.x = oldHead;
	newNode.y = 0;
    newNode.z = floatBitsToUint(vPositionView.z);
    newNode.w = 0;
    
    imageStore(gpuMemoryPool, int(newHead), newNode);

	gl_FragData[0] = vec4(materialColor, 0.0);
}
