#version 120

varying vec4 vPositionView;
varying vec4 vNormalView;

uniform vec3 materialColor;

void main()
{
	gl_FragData[0] = vec4(materialColor, 1.0);
}
