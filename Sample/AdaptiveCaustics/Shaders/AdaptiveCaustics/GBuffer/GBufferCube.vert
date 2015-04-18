#version 430 core

attribute vec4 vPosition;
attribute vec3 vNormal;

out vec4 vPositionView;
out vec4 vPositionWorld;
out vec4 vNormalView;
out vec4 vNormalWorld;
out vec3 pTCoord;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vNormalWorld = World * vec4(vNormal, 0.0);
	vNormalView = View * World * vec4(vNormal, 0.0);

	vPositionWorld = World * vPosition;
	vPositionView = View * vPositionWorld;

    gl_Position =  Proj * vPositionView;
	pTCoord = vPosition.xyz;
}
