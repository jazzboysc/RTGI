#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

// World space ray.
uniform vec3 rayStartPoint;
uniform vec3 rayEndPoint;

void main()
{
    float minT, maxT, sceneMinT, sceneMaxT;

    // Transform world space ray to SVO space.
    vec3 sceneBBMin = voxelFragmentBuffer.SceneBBMin.xyz;
    vec3 rayStartPosSVO = rayStartPoint - sceneBBMin;
    vec3 rayEndPosSVO = rayEndPoint - sceneBBMin;

    // Initialize sceneMinT, sceneMaxT and ray direction.
    vec3 rayDirSVO = rayEndPosSVO - rayStartPosSVO;
    sceneMinT = 0.0;
    sceneMaxT = length(rayDirSVO);
    rayDirSVO = rayDirSVO / sceneMaxT;
    vec3 rayInvDirSVO = 1.0 / rayDirSVO;

    bool hit = false;
    minT = maxT = sceneMinT;
    SVONode curNode, root;
    vec3 rayEntryPos;

    // Initialize root node.
    root.child = 0;
    root.flag = SVO_NODE_FLAGED;
    root.nodeBox.Min = Ivec3ToUint(ivec3(0, 0, 0));
    root.nodeBox.Max = Ivec3ToUint(ivec3(SVO_MAX_LEVEL_DIM,
        SVO_MAX_LEVEL_DIM, SVO_MAX_LEVEL_DIM));

    uint level, childIndex, nextNodeIndex;
    while( maxT < sceneMaxT )
    {
        // Restart traversal from root.
        curNode = root;
        minT = maxT;
        maxT = sceneMaxT;
        level = 0;
        rayEntryPos = rayStartPosSVO + rayDirSVO*minT;

        // Descend until we meet a leaf node.
        while( !IsSVOLeafNode(curNode) )
        {
            // Figure out which child node we are in.
            childIndex = GetSVOChildNodeIndex(rayEntryPos, curNode.nodeBox);

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

        // Find maxT for the current node.
        SVORayBoxIntersection(rayStartPosSVO, rayInvDirSVO, minT, maxT, 
            curNode.nodeBox, minT, maxT);
    }
}