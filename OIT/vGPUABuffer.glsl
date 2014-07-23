#version 120

attribute vec4 vPosition;
attribute vec3 vNormal;

varying vec4 vNormalView;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vNormalView = View * World * vec4(vNormal, 0.0);
    gl_Position =  Proj * View * World * vPosition;
}
