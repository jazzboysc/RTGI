#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

// World space ray.
uniform vec3 rayStartPoint;
uniform vec3 rayEndPoint;

void main()
{
    float tMin, tMax, sceneMin, sceneMax;

    // Transform world space ray to SVO space.
    vec3 sceneBBMin = voxelFragmentBuffer.SceneBBMin.xyz;
    vec3 rayStartPosSVO = rayStartPoint - sceneBBMin;
    vec3 rayEndPosSVO = rayEndPoint - sceneBBMin;
    ivec3 rayStartGridPos = WorldToGridPosition(rayStartPoint.xyz);

    // Initialize sceneMin, sceneMax and ray direction.
    vec3 rayDirection = rayEndPosSVO - rayStartPosSVO;
    sceneMin = 0.0;
    sceneMax = length(rayDirection);
    rayDirection = rayDirection / sceneMax;

    bool hit = false;
    vec3 rayPos = rayStartPoint;
    tMin = tMax = sceneMin;
    SVONode curNode, root;

    // Initialize root node.
    root.child = 0;
    root.flag = SVO_NODE_FLAGED;
    root.nodeBox.Min = Ivec3ToUint(ivec3(0, 0, 0));
    root.nodeBox.Max = Ivec3ToUint(ivec3(SVO_MAX_LEVEL_DIM,
        SVO_MAX_LEVEL_DIM, SVO_MAX_LEVEL_DIM));

    uint level, childIndex, nextNodeIndex;
    while( tMax < sceneMax )
    {
        // Restart traversal from root.
        curNode = root;
        tMin = tMax;
        tMax = sceneMax;
        level = 0;

        // Descend until we meet a leaf node.
        while( !IsSVOLeafNode(curNode) )
        {
            // Figure out which child node we are in.
            childIndex = GetSVOChildNodeIndex(rayStartGridPos, curNode.nodeBox);

            // Locate child node for current tree level.
            nextNodeIndex = curNode.child*SVO_NODE_TILE_SIZE + childIndex;

            // Update next node to visit.
            curNode = svoNodeBuffer.data[nextNodeIndex];

            level++;
        }

        // Deal with the leaf node.
        if( level == SVO_MAX_LEVEL )
        {
            hit = true;
            break;
        }
    }
}