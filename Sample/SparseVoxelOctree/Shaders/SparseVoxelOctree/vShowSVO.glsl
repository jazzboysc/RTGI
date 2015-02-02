#version 430 core

in vec4 vPosition;
in vec3 vNormal;

out vec4 boxColor;

#include "SparseVoxelOctree/sSparseVoxelOctree.glsl"

uniform mat4 View;
uniform mat4 Proj;

uniform vec3 SceneBBMin;
uniform vec3 VoxelExtension;

mat4 GetSVONodeWorldTransform(SVONode node)
{
    mat4 res;

    ivec3 nodeBoxMin = UintToIvec3(node.nodeBox.Min);
    ivec3 nodeBoxMax = UintToIvec3(node.nodeBox.Max);
    ivec3 nodeBoxMid = (nodeBoxMax + nodeBoxMin) >> 1;
    ivec3 nodeBoxExtension = nodeBoxMax - nodeBoxMin;

    vec3 nodeBoxWorldExtension = nodeBoxExtension * VoxelExtension;
    vec3 nodeBoxWorldMid = SceneBBMin + nodeBoxMid*VoxelExtension;

    nodeBoxWorldExtension *= 0.5; // Because we are using cube of length 2.

    res = mat4(vec4(nodeBoxWorldExtension.x, 0.0, 0.0, 0.0),
               vec4(0.0, nodeBoxWorldExtension.y, 0.0, 0.0),
               vec4(0.0, 0.0, nodeBoxWorldExtension.z, 0.0),
               vec4(nodeBoxWorldMid.x, nodeBoxWorldMid.y, nodeBoxWorldMid.z, 1.0));

    return res;
}

void main()
{
    SVONode node = svoNodeBuffer.data[gl_InstanceID];
    if( node.flag == SVO_NODE_FLAGED )
    {
        boxColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
    else
    {
        boxColor = vec4(0.0, 1.0, 0.0, 1.0);
    }

    mat4 World = GetSVONodeWorldTransform(node);
    gl_Position = Proj * View * World * vPosition;
}