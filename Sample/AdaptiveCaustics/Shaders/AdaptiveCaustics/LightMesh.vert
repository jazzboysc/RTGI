#version 430 core

in vec4 vPosition;
in vec2 vTCoord;

out vec2 pTCoord;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
    gl_Position = Proj * View * World * vPosition;
    pTCoord = vTCoord;
}
