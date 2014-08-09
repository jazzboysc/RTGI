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

layout (binding = 0, r32ui)   uniform uimage2D headPointerImage;

layout (std430, binding = 0)  buffer gpuMemoryPool1
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

layout (location = 0) out vec4 color;

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

vec3 GetRadianceFromAccumulationBuffer(ListNode node)
{
	ivec3 coords = GetImageCoords(node.worldPosition);
	int index = coords.x + (coords.y + coords.z*256)*256;
    vec3 radiance = accumulationBuffer.data[index].radiance;
	return radiance;
}

void AddRadianceToAccumulationBuffer(ListNode node, vec3 radiance)
{
    vec3 currentRadiance = GetRadianceFromAccumulationBuffer(node);
	currentRadiance += radiance;
	ivec3 coords = GetImageCoords(node.worldPosition);
	int index = coords.x + (coords.y + coords.z*256)*256;
	accumulationBuffer.data[index].radiance = currentRadiance;
	//accumulationBuffer.data[index].radiance = radiance;
}

void TransferEnergy(ListNode receiver, ListNode sender)
{
    vec3 incidentDir = receiver.worldPosition - sender.worldPosition;
    float incidentDirLengthSqr = dot(incidentDir, incidentDir);
    vec3 normalizedIncidentDir = normalize(incidentDir);
    
    //float geometricTerm = max(0.0, dot(-normalizedIncidentDir, receiver.worldNormal)) *
    //    max(0.0, dot(normalizedIncidentDir, sender.worldNormal)) /
    //    incidentDirLengthSqr;
    float geometricTerm = max(0.0, dot(-normalizedIncidentDir, vec3(0.0, 1.0, 0.0))) *
        max(0.0, dot(normalizedIncidentDir, vec3(0.0, -1.0, 0.0)));
	geometricTerm /= incidentDirLengthSqr;
    
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
            
            if( depthI < depthJ )
            {
                ListNode temp = perPixelList[i];
                perPixelList[i] = perPixelList[j];
                perPixelList[j] = temp;
            }
        }
    }

	//for( int i = 0; i < nodeCount; ++i )
	//{
	//	vec3 normal = perPixelList[i].worldNormal.xyz;
	//	normal = normal*0.5 + 0.5;
	//	//AddRadianceToAccumulationBuffer(perPixelList[i], perPixelList[i].materialColor);
	//	AddRadianceToAccumulationBuffer(perPixelList[i], normal);
	//}

	// Transfer energy between two consecutive intersection points.
	if( nodeCount >= 2 )
	{
		ListNode receiver, sender;

		// Handle first intersection point.
		receiver = perPixelList[0];
		sender = perPixelList[1];
		//float d = dot(worldRayBundleDirection, receiver.worldNormal);
		float d = dot(worldRayBundleDirection, vec3(0.0, 1.0, 0.0));
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
