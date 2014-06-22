#version 120

varying vec4 vPositionView;
varying vec4 vNormalView;

uniform vec3 materialColor;

void main()
{
	vec3 vNormal = normalize(vNormalView.xyz);
	vec3 vLightDir = normalize(-vPositionView.xyz);
	float cosAlpha = max(0.0, vLightDir.z); // light space z axis is (0, 0, 1) in light space, cosAlpha = max(0.0, dot(light z axis, light dir) 

	gl_FragData[0] = vPositionView;
	gl_FragData[1] = vec4(vNormal, 0.0);
	gl_FragData[2] = vec4(materialColor*cosAlpha, cosAlpha);
}
