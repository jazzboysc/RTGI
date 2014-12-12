#version 430 core

attribute vec4 vPosition;
attribute vec3 vNormal;

varying vec4 vPositionWorld;
varying vec4 vPositionView;
varying vec4 vNormalWorld;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vPositionWorld = World * vPosition;
	vPositionView = View * vPositionWorld;
	vNormalWorld = World * vec4(vNormal, 0.0);
    gl_Position =  Proj * View * vPositionWorld;
}