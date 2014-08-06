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

vec3 GetRadianceFromAccumulationBuffer(ListNode sender)
{
    // TODO:
}

void AddRadianceToAccumulationBuffer(ListNode receiver, vec3 radiance)
{
    // TODO:
}

void TransferEnergy(ListNode receiver, ListNode sender)
{
    vec3 incidentDir = receiver.worldPosition - sender.worldPosition;
    float incidentDirLengthSqr = dot(incidentDir, incidentDir);
    vec3 normalizedIncidentDir = normalize(incidentDir);
    
    float geometricTerm = max(0.0, dot(-normalizedIncidentDir, receiver.worldNormal)) *
        max(0.0, dot(normalizedIncidentDir, sender.worldNormal)) /
        incidentDirLengthSqr;
    
    vec3 senderRadiance;
    if( sender.isLight )
    {
        senderRadiance = sender.emissionColor;
    }
    else
    {
        senderRadiance = GetRadianceFromAccumulationBuffer(sender);
    }
    
    vec3 receiverRadiance = senderRadiance * geometricTerm;
    AddRadianceToAccumulationBuffer(receiver, receiverRadiance);
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

	// Transfer energy between two consecutive intersection points.
	if( nodeCount >= 2 )
	{
		ListNode receiver, sender;

		// Handle first intersection point.
		receiver = perPixelList[0];
		sender = perPixelList[1];
		float d = dot(worldRayBundleDirection, receiver.worldNormal);
		if( d > 0.0 )
		{
			TransferEnergy(receiver, sender);
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
			TransferEnergy(receiver, sender);
		}

		// Handle last intersection point.
		receiver = perPixelList[i];
		sender = perPixelList[i - 1];
		d = dot(worldRayBundleDirection, receiver.worldNormal);
		if( d < 0.0 )
		{
			TransferEnergy(receiver, sender);
		}
	}
}
