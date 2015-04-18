#version 430 core

attribute vec4 vPosition;
attribute vec3 vNormal;

out vec4 vPositionView;
out vec4 vPositionWorld;
out vec4 vNormalView;
out vec4 vNormalWorld;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vNormalView = View * World * vec4(vNormal, 0.0);
	vNormalWorld = World * vec4(vNormal, 0.0);
	vPositionView = View * World * vPosition;
	vPositionWorld = World * vPosition;
    gl_Position =  Proj * vPositionView;
}
