#version 430 core

in vec4 vPosition;

out float vViewPosZ;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
    vec4 viewPos = View * World * vPosition;
    vViewPosZ = viewPos.z;
    gl_Position = Proj * viewPos;
}