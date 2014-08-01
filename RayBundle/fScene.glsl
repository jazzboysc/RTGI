#version 120

varying vec4 vPositionView;
varying vec4 vNormalView;

uniform vec3 materialColor;
uniform vec3 emissionColor;

void main()
{
	vec3 vNormal = normalize(vNormalView.xyz);
	vec3 vLightDir = normalize(-vPositionView.xyz);

	gl_FragData[0] = vec4(materialColor, 1.0);
}
