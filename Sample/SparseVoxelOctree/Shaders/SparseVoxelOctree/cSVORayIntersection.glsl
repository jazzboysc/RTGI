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

    // Debug.
    //svoNodeBuffer.sceneBBMin = vec4(sceneBBMin, 1.0);
    //svoNodeBuffer.rayStartPos = vec4(rayStartPoint, 1.0);
    //svoNodeBuffer.rayEndPos = vec4(rayEndPoint, 1.0);

    // Initialize sceneMinT, sceneMaxT and ray direction.
    vec3 rayDirSVO = rayEndPosSVO - rayStartPosSVO;
    sceneMinT = 0.0;
    sceneMaxT = length(rayDirSVO);
    rayDirSVO = rayDirSVO / sceneMaxT;
    vec3 rayInvDirSVO = 1.0 / rayDirSVO;

    uint hit = 0;
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
    //while( maxT < sceneMaxT )
    for( int i = 0; i < 1; ++i )
    {
        // Restart traversal from root.
        curNode = root;
        minT = maxT;
        maxT = sceneMaxT;
        level = 0;
        rayEntryPos = rayStartPosSVO + rayDirSVO*minT;

        // Debug.
        svoNodeBuffer.minT = minT;
        svoNodeBuffer.maxT = maxT;
        svoNodeBuffer.rayEntryPos = vec4(rayEntryPos, 1.0);

        // Descend until we meet a leaf node.
        bool isLeaf = IsSVOLeafNode(curNode);

        // Debug.
        //if( isLeaf )
        //{
        //    svoNodeBuffer.isLeaf = 1;
        //}
        //else
        //{
        //    svoNodeBuffer.isLeaf = 0;
        //}

        //while( !isLeaf )
        for( int j = 0; j < 1; ++j )
        {
            // Figure out which child node we are in.
            childIndex = GetSVOChildNodeIndex2(rayEntryPos, curNode.nodeBox);

            // Locate child node for current tree level.
            nextNodeIndex = curNode.child*SVO_NODE_TILE_SIZE + childIndex;

            // Update next node to visit.
            curNode = svoNodeBuffer.data[nextNodeIndex];

            isLeaf = IsSVOLeafNode(curNode);
            level++;

            // Debug.
            svoNodeBuffer.childIndex = childIndex;
            if( isLeaf )
            {
                svoNodeBuffer.isLeaf = 1;
            }
            else
            {
                svoNodeBuffer.isLeaf = 0;
            }
        }

        // Deal with the leaf node.
        if( level == 2 )
        {
            hit = 1;
            break;
        }

        // Find maxT for the current node.
        SVORayBoxIntersection(rayStartPosSVO, rayInvDirSVO, minT, maxT, 
            curNode.nodeBox, minT, maxT);
    }

    svoNodeBuffer.hit = hit;
}