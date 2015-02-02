#version 430 core

in vec4 boxColor;

void main()
{
    gl_FragData[0] = boxColor;
}