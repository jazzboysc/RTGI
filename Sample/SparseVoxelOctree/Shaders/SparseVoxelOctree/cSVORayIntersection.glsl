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
    vec3 inv2SceneBBExtension = voxelFragmentBuffer.Inv2SceneBBExtension.xyz;
    vec3 rayStartPosSVO = (rayStartPoint - sceneBBMin)*inv2SceneBBExtension*SVO_MAX_LEVEL_DIM;
    vec3 rayEndPosSVO = (rayEndPoint - sceneBBMin)*inv2SceneBBExtension*SVO_MAX_LEVEL_DIM;

    // Initialize sceneMinT, sceneMaxT and ray direction.
    vec3 rayDirSVO = rayEndPosSVO - rayStartPosSVO;
    sceneMinT = 0.0;
    sceneMaxT = length(rayDirSVO);
    rayDirSVO = rayDirSVO / sceneMaxT;
    vec3 rayInvDirSVO = 1.0 / rayDirSVO;

    // Debug.
    //svoNodeBuffer.sceneBBMin = vec4(sceneBBMin, 1.0);
    //svoNodeBuffer.rayStartPos = vec4(rayStartPoint, 1.0);
    //svoNodeBuffer.rayEndPos = vec4(rayEndPoint, 1.0);
    //svoNodeBuffer.rayStartPosSVO = vec4(rayStartPosSVO, 1.0);
    //svoNodeBuffer.rayEndPosSVO = vec4(rayEndPosSVO, 1.0);
    //svoNodeBuffer.rayInvDirSVO = vec4(rayInvDirSVO, 0.0);

    uint hit = 0;
    minT = maxT = sceneMinT;
    SVONode curNode, root;
    vec3 rayEntryPos;

    // Initialize root node.
    root.info = SVO_NODE_FLAG_MASK;
    root.nodeBox.Min = Ivec3ToUint(ivec3(0, 0, 0));
    root.nodeBox.Max = Ivec3ToUint(ivec3(SVO_MAX_LEVEL_DIM,
        SVO_MAX_LEVEL_DIM, SVO_MAX_LEVEL_DIM));

    uint level, childIndex, nextNodeIndex;
    while( maxT < sceneMaxT )
    //for( int i = 0; i < 5; ++i )
    {
        // Restart traversal from root.
        curNode = root;
        minT = maxT;
        maxT = sceneMaxT;
        rayEntryPos = rayStartPosSVO + rayDirSVO*minT;

        // Debug.
        //if( i == 4 )
        //{
        //    svoNodeBuffer.minT = minT;
        //    svoNodeBuffer.maxT = maxT;
        //    svoNodeBuffer.rayEntryPos = vec4(rayEntryPos, 1.0);
        //}

        // Descend until we meet a leaf node.
        bool isLeaf = IsSVOLeafNode(curNode);
        while( !isLeaf )
        //for( int j = 0; j < 2; ++j )
        {
            // Figure out which child node we are in.
            childIndex = GetSVOChildNodeIndex(rayEntryPos, curNode.nodeBox);

            // Locate child node for current tree level.
            uint childrenID = (curNode.info & SVO_NODE_CHILDREN_ID_MASK);
            nextNodeIndex = childrenID*SVO_NODE_TILE_SIZE + childIndex;

            // Debug.
            //if( j == 1 )
            //{
            //    vec3 nodeBoxMin, nodeBoxMax, mid;
            //    nodeBoxMin = vec3(UintToIvec3(curNode.nodeBox.Min));
            //    nodeBoxMax = vec3(UintToIvec3(curNode.nodeBox.Max));
            //    mid = (nodeBoxMin + nodeBoxMax)*0.5;

            //    svoNodeBuffer.nodeBoxMin = vec4(nodeBoxMin, 1.0);
            //    svoNodeBuffer.nodeBoxMax = vec4(nodeBoxMax, 1.0);
            //    svoNodeBuffer.mid = vec4(mid, 1.0);
            //    svoNodeBuffer.childIndex = childIndex;
            //    svoNodeBuffer.childTileIndex = (curNode.info & SVO_NODE_CHILDREN_ID_MASK);
            //    svoNodeBuffer.nextNodeIndex = nextNodeIndex;

            //    if( isLeaf )
            //    {
            //        svoNodeBuffer.isLeaf = 1;
            //    }
            //    else
            //    {
            //        svoNodeBuffer.isLeaf = 0;
            //    }
            //}

            // Update next node to visit.
            curNode = svoNodeBuffer.data[nextNodeIndex];
            isLeaf = IsSVOLeafNode(curNode);
        }

        // Deal with the leaf node.
        if( IsSVONodeFlaged(curNode) )
        {
            hit = 1;
            break;
        }

        // Find maxT for the current node.

        vec3 nodeBoxMin = vec3(UintToIvec3(curNode.nodeBox.Min));
        vec3 nodeBoxMax = vec3(UintToIvec3(curNode.nodeBox.Max));
        vec3 minMinusOrigin = nodeBoxMin - rayStartPosSVO;
        vec3 maxMinusOrigin = nodeBoxMax - rayStartPosSVO;
        float t0 = minT, t1 = maxT;

        for( int k = 0; k < 3; ++k )
        {
            float tNear = minMinusOrigin[k] * rayInvDirSVO[k];
            float tFar = maxMinusOrigin[k] * rayInvDirSVO[k];

            // Debug.
            //if( i == 0 && k == 1 )
            //{
            //    svoNodeBuffer.nodeBoxMin = vec4(nodeBoxMin, 1.0);
            //    svoNodeBuffer.nodeBoxMax = vec4(nodeBoxMax, 1.0);
            //    svoNodeBuffer.minMinusOriginK = minMinusOrigin[k];
            //    svoNodeBuffer.maxMinusOriginK = maxMinusOrigin[k];
            //    svoNodeBuffer.tNear = tNear;
            //    svoNodeBuffer.tFar = tFar;

            //    if( tNear > tFar )
            //    {
            //        svoNodeBuffer.tNearCompareTFar = 1;
            //    }
            //    else
            //    {
            //        svoNodeBuffer.tNearCompareTFar = 0;
            //    }
            //}

            if( tNear > tFar )
            {
                // Swap.
                float temp = tNear;
                tNear = tFar;
                tFar = temp;
            }

            t0 = tNear > t0 ? tNear : t0;
            t1 = tFar  < t1 ? tFar : t1;

            if( t0 > t1 )
            {
                return;
            }
        }

        // Update maxT.
        maxT = t1 + SVO_RAY_T_EPSILON;

    }

    svoNodeBuffer.hit = hit;
}