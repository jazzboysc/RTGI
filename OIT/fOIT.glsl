#version 420 core

#define LIST_MAX_LENGTH 20

layout (binding = 0, r32ui) uniform uimage2D headPointerImage;
layout (binding = 1, rgba32ui) uniform uimageBuffer gpuMemoryPool;

uvec4 perPixelNodes[LIST_MAX_LENGTH];

layout (location = 0) out vec4 color;

int GetPerPixelNodes(ivec2 pixelCoord)
{
    uint currentNodeIndex;
    int nodeCount = 0;
    
    // Get linked list header which is the first node index.
    currentNodeIndex = imageLoad(headPointerImage, pixelCoord).x;
    
    while( currentNodeIndex != 0xFFFFFFFF && nodeCount < LIST_MAX_LENGTH )
    {
        // Get a previously stored node from GPU buffer.
        uvec4 currentNode = imageLoad(gpuMemoryPool, int(currentNodeIndex));
        
        // Get next node index linked by current node.
        currentNodeIndex = currentNode.x;
        
        // Store the node in our global array.
        perPixelNodes[nodeCount] = currentNode;
        
        nodeCount++;
    }
    
    return nodeCount;
}

void SortPerPixelNodes(int nodeCount)
{
    for( int i = 0; i < nodeCount; ++i )
    {
        for( int j = i + 1; j < nodeCount; ++j )
        {
            float depthI = uintBitsToFloat(perPixelNodes[i].z);
            float depthJ = uintBitsToFloat(perPixelNodes[j].z);
            
            if( depthI > depthJ )
            {
                uvec4 temp = perPixelNodes[i];
                perPixelNodes[i] = perPixelNodes[j];
                perPixelNodes[j] = temp;
            }
        }
    }
}

vec4 BlendTwoColor(vec4 lhsColor, vec4 rhsColor)
{
    return mix(lhsColor, rhsColor, rhsColor.a);
}

vec4 BlendPerPixelNodes(int nodeCount)
{
    vec4 color = vec4(0.0);
    
    for( int i = 0; i < nodeCount; ++i )
    {
        vec4 nodeColor = unpackUnorm4x8(perPixelNodes[i].y);
        color = BlendTwoColor(color, nodeColor);
    }
    
    return color;
}

void main()
{
    int nodeCount = GetPerPixelNodes(ivec2(gl_FragCoord.xy));
    SortPerPixelNodes(nodeCount);
    color = BlendPerPixelNodes(nodeCount);
}
