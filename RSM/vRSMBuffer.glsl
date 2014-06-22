#version 120

attribute vec4 vPosition;
attribute vec3 vNormal;

varying vec4 vPositionView;
varying vec4 vNormalView;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vNormalView = View * World * vec4(vNormal, 0.0);
    vPositionView = View * World * vPosition;
	vec4 vPositionH = Proj * vPositionView;
	//vPositionView.xyzw = vPositionH.xyzw / vPositionH.w;
	//vPositionView.xy = (vPositionView.xy+1.0)*0.5;
    gl_Position =  vPositionH;
}
