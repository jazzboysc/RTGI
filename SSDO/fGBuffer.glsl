#version 120

varying vec4 vPositionView;
varying vec4 vNormalView;

uniform vec3 materialColor;

void main()
{
	vec3 vNormal = normalize(vNormalView.xyz);
	vNormal.xyz = (vNormal.xyz + 1.0) * 0.5;

	gl_FragData[0] = vPositionView;
	gl_FragData[1] = vec4(vNormal, 0.0);
	gl_FragData[2] = vec4(materialColor, 1.0);
}
