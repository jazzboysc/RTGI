#version 420 core

attribute vec4 vPosition;
attribute vec2 vTCoord;

void main()
{
    gl_Position = vPosition;
	gl_Position.w = 1.0;
}
