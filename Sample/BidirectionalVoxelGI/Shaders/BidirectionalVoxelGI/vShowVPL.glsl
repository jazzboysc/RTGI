#version 430 core

#include "BidirectionalVoxelGI/sVirtualPointLight.glsl"

in vec4 vPosition;

uniform mat4 View;
uniform mat4 Proj;

uniform int CurVPLSubsetID;
uniform int VPLCount;
uniform int PatternSize;

void main()
{
    int subsetSize = VPLCount / (PatternSize*PatternSize);
    int baseIndex = subsetSize*CurVPLSubsetID;
    vec4 worldPosition = VPLBuffer.vpls[baseIndex + gl_InstanceID].WorldPosition;
    gl_Position =  Proj * View * worldPosition;
}