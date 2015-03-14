#version 120

varying vec4 vPositionView;
varying vec4 vPositionWorld;
varying vec4 vNormalView;
varying vec4 vNormalWorld;

uniform vec3 materialColor;

void main()
{
	vec3 vNormal = normalize(vNormalWorld.xyz);
	//vNormal.xyz = (vNormal.xyz + 1.0) * 0.5;

	//gl_FragData[0] = vPositionWorld;
	gl_FragData[0] = vec4(vNormal, 1.0);
	//gl_FragData[2] = vec4(materialColor, 1.0);
}
