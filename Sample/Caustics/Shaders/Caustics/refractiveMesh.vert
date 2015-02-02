#version 120

attribute vec4 vPosition;
attribute vec2 vTCoord;
varying vec2 pTCoord;

void main()
{
    gl_Position = vPosition;
	gl_Position.w = 1.0;
	pTCoord = vTCoord;
}
