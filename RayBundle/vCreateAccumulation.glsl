#version 430 core

attribute vec4 vPosition;
attribute vec3 vNormal;

varying vec4 vPositionView;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vPositionView = View * World * vPosition;
    gl_Position =  Proj * vPositionView;
}
