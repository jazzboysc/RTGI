#version 120

attribute vec4 vPosition;
attribute vec3 vNormal;

varying vec4 vPositionView;
varying vec4 vNormalView;
varying vec4 vPositionWorld;
varying vec4 vNormalWorld;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vNormalWorld = World * vec4(vNormal, 0.0);
    vPositionWorld = World * vPosition;
	vNormalView = View * vNormalWorld;
    vPositionView = View * vPositionWorld;
	vec4 vPositionH = Proj * vPositionView;
    gl_Position =  vPositionH;
}
