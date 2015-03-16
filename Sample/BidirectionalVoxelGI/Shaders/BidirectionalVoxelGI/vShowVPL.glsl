#version 430 core

#include "BidirectionalVoxelGI/sVirtualPointLight.glsl"

in vec4 vPosition;

uniform mat4 View;
uniform mat4 Proj;

void main()
{
    vec4 worldPosition = VPLBuffer.vpls[gl_VertexID].WorldPosition;
    gl_Position =  Proj * View * worldPosition;
}