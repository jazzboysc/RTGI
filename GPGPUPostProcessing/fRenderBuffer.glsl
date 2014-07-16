#version 120

varying vec4 vNormalView;

void main()
{
	vec3 vNormal = normalize(vNormalView.xyz);
	vNormal.xyz = (vNormal.xyz + 1.0) * 0.5;

	gl_FragData[0] = vec4(vNormal, 0.0);
}
