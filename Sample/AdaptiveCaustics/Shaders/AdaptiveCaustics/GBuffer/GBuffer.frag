#version 430 core

in vec4 vPositionView;
in vec4 vPositionWorld;
in vec4 vNormalView;
in vec4 vNormalWorld;

uniform vec3 materialColor;

void main()
{
	vec3 vNormal = normalize(vNormalWorld.xyz);
	//vNormal.xyz = (vNormal.xyz + 1.0) * 0.5;

	gl_FragData[0] = vPositionWorld;
	gl_FragData[1] = vec4(vNormal, 1.0);
	gl_FragData[2] = vec4(materialColor, 1.0);
}
