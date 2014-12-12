#version 430 core

struct ListNode
{
	uint next;
	vec4 data;
	float depth;
};

#define LIST_MAX_LENGTH 20
//uvec4 perPixelList[LIST_MAX_LENGTH];
ListNode perPixelList[LIST_MAX_LENGTH];

layout (binding = 0, r32ui)    uniform uimage2D     headPointerImage;
//layout (binding = 1, rgba32ui) uniform uimageBuffer gpuMemoryPool;
layout (location = 0) out vec4 color;

layout (std430, binding = 0) buffer gpuMemoryPool2
{
	ListNode nodes[];
}outBuffer;

void main()
{
    uint currentNodeIndex;
    int nodeCount = 0;
    
    // Get linked list header which is the first node index.
    currentNodeIndex = imageLoad(headPointerImage, ivec2(gl_FragCoord.xy)).x;
    while( currentNodeIndex != 0 && nodeCount < LIST_MAX_LENGTH )
    {
        // Get a previously stored node from GPU buffer.
        //uvec4 currentNode = imageLoad(gpuMemoryPool, int(currentNodeIndex));
		ListNode currentNode = outBuffer.nodes[currentNodeIndex];
        
        // Get next node index linked by current node.
        //currentNodeIndex = currentNode.x;
		currentNodeIndex = currentNode.next;
        
        // Store the node in our global array.
        perPixelList[nodeCount] = currentNode;
        
        nodeCount++;
    }

	// Sort the per-pixel list base on node depth value.
    for( int i = 0; i < nodeCount; ++i )
    {
        for( int j = i + 1; j < nodeCount; ++j )
        {
            //float depthI = uintBitsToFloat(perPixelList[i].z);
            //float depthJ = uintBitsToFloat(perPixelList[j].z);
            float depthI = perPixelList[i].depth;
            float depthJ = perPixelList[j].depth;
            
            if( depthI < depthJ )
            {
                //uvec4 temp = perPixelList[i];
				ListNode temp = perPixelList[i];
                perPixelList[i] = perPixelList[j];
                perPixelList[j] = temp;
            }
        }
    }

	// Blend all the nodes for current pixel.
    color = vec4(0.5);
    for( int i = nodeCount - 1; i >= 0; --i )
    {
        //vec4 nodeColor = unpackUnorm4x8(perPixelList[i].y);
		vec4 nodeColor = perPixelList[i].data;
		color = mix(color, nodeColor, nodeColor.a);
    }
}
