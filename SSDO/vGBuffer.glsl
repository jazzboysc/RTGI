#version 150

in vec4 vPosition;
in vec3 vNormal;

varying vec4 vPositionView;
varying vec4 vNormalView;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vNormalView = View * World * vec4(vNormal, 0.0);
	vPositionView = View * World * vPosition;
    gl_Position =  Proj * vPositionView;
}
