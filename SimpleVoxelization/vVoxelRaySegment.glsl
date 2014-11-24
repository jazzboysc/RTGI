#version 430 core

in vec4 vPosition;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
    gl_Position = Proj * View * World * vPosition;
}
