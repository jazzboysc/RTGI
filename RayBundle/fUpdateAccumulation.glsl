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

uniform vec3 worldRayBundleDirection;

#define LIST_MAX_LENGTH 20
ListNode perPixelList[LIST_MAX_LENGTH];

layout (binding = 0, r32ui) uniform uimage2D headPointerImage;
layout (location = 0) out vec4 color;
layout (std430, binding = 0) buffer gpuMemoryPool
{
	ListNode nodes[];
} rayBundleBuffer;

void ExchangeEnergy(ListNode receiver, ListNode sender)
{
	// TODO.
}

void main()
{
    uint currentNodeIndex;
    int nodeCount = 0;
    
    // Get linked list header which is the first node index.
    currentNodeIndex = imageLoad(headPointerImage, ivec2(gl_FragCoord.xy)).x;
    while( currentNodeIndex != 0 && nodeCount < LIST_MAX_LENGTH )
    {
        // Get a previously stored node from GPU buffer.
        ListNode currentNode = rayBundleBuffer.nodes[currentNodeIndex];
        
        // Get next node index linked by current node.
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
            float depthI = perPixelList[i].depth;
            float depthJ = perPixelList[j].depth;
            
            if( depthI > depthJ )
            {
                ListNode temp = perPixelList[i];
                perPixelList[i] = perPixelList[j];
                perPixelList[j] = temp;
            }
        }
    }

	// Exchange energy between two consecutive intersection points.
	if( nodeCount >= 2 )
	{
		ListNode receiver, sender;

		// Handle first intersection point.
		receiver = perPixelList[0];
		sender = perPixelList[1];
		float d = dot(worldRayBundleDirection, receiver.worldNormal);
		if( d > 0.0 )
		{
			ExchangeEnergy(receiver, sender);
		}

		int i;
		for( i = 1; i < nodeCount - 1; ++i )
		{
			receiver = perPixelList[i];
			d = dot(worldRayBundleDirection, receiver.worldNormal);
			if( d > 0.0 )
			{
				sender = perPixelList[i + 1];
			}
			else
			{
				sender = perPixelList[i - 1];
			}
			ExchangeEnergy(receiver, sender);
		}

		// Handle last intersection point.
		receiver = perPixelList[i];
		sender = perPixelList[i - 1];
		d = dot(worldRayBundleDirection, receiver.worldNormal);
		if( d < 0.0 )
		{
			ExchangeEnergy(receiver, sender);
		}
	}
}
