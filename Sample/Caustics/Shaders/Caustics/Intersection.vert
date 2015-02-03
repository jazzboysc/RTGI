#version 120

attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec2 vTCoord;

varying vec2 pTCoord;
varying vec4 vPositionWorld;
varying vec4 vNormalWorld;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
    gl_Position = vPosition;
	gl_Position.w = 1.0;

	vNormalWorld = World * vec4(vNormal, 0.0);
	vPositionWorld = World * vPosition;
	pTCoord = vTCoord;
}
