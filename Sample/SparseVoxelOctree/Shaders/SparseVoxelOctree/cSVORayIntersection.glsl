#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

uniform vec3 rayStartPoint;
uniform vec3 rayEndPoint;

void main()
{
    float tMin, tMax, sceneMin, sceneMax;

    // Initialize sceneMin, sceneMax and ray direction.
    vec3 rayDirection = rayEndPoint - rayStartPoint;
    sceneMin = 0.0;
    sceneMax = length(rayDirection);
    rayDirection = rayDirection / sceneMax;

    bool hit = false;
    uint level = 0;
    vec3 rayPos = rayStartPoint;
    tMin = tMax = sceneMin;
    SVONode node, root;

    // Initialize root node.
    root.child = 0;
    root.flag = SVO_NODE_FLAGED;
    root.nodeBox.Min = Ivec3ToUint(ivec3(0, 0, 0));
    root.nodeBox.Max = Ivec3ToUint(ivec3(SVO_MAX_LEVEL_DIM,
        SVO_MAX_LEVEL_DIM, SVO_MAX_LEVEL_DIM));

    while( tMax < sceneMax )
    {
        // Restart traversal from root.
        node = root;
        tMin = tMax;
        tMax = sceneMax;

        // Traverse until we meet a leaf node.
        while( !IsLeaf(node) )
        {
            // Figure out which child node we are in.
            childIndex = GetSVOChildNodeIndex(rayPos, nodeBox);
        }

        // Deal with the leaf node.
        if( level == SVO_MAX_LEVEL )
        {
            hit = true;
            break;
        }
    }
}