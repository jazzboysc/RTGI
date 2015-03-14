#version 120

varying vec4 vPositionView;
varying vec4 vPositionWorld;
varying vec4 vNormalView;
varying vec4 vNormalWorld;

uniform vec3 materialColor;
varying vec3 pTCoord;
uniform samplerCube cubemapSampler;

void main()
{
	vec3 vNormal = normalize(vNormalWorld.xyz);
	//vNormal.xyz = (-vNormal.xyz + 1.0) * 0.5;
	vec4 texColor = textureCube(cubemapSampler, pTCoord);

	gl_FragData[0] = vPositionWorld;
	// Flip normal because we want the light to affect the inside of the cube
	gl_FragData[1] = vec4(vNormal, 1.0);
	gl_FragData[2] = vec4(texColor);
}
