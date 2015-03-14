#version 120

attribute vec4 vPosition;
attribute vec3 vNormal;

varying vec4 vPositionView;
varying vec4 vPositionWorld;
varying vec4 vNormalView;
varying vec4 vNormalWorld;
varying vec3 pTCoord;

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
