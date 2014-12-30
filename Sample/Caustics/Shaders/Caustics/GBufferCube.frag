#version 120

varying vec4 vPositionView;
varying vec4 vNormalView;
varying vec4 vNormalWorld;
varying vec4 vPositionWorld;

uniform vec3 materialColor;
varying vec3 pTCoord;
uniform samplerCube cubemapSampler;

void main()
{
	vec3 vNormal = normalize(vNormalWorld.xyz);
	vNormal.xyz = (vNormal.xyz + 1.0) * 0.5;
	vec4 texColor = textureCube(cubemapSampler, pTCoord);

	gl_FragData[0] = vPositionWorld;
	gl_FragData[1] = vec4(vNormal, 0.0);
	gl_FragData[2] = vec4(texColor);
}
