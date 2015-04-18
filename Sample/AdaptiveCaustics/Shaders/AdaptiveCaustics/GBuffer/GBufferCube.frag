#version 430 core

in vec4 vPositionView;
in vec4 vPositionWorld;
in vec4 vNormalView;
in vec4 vNormalWorld;
in vec3 pTCoord;

uniform vec3 materialColor;
uniform samplerCube cubemapSampler;

void main()
{
	vec3 vNormal = normalize(vNormalWorld.xyz);
	//vNormal.xyz = (-vNormal.xyz + 1.0) * 0.5;
	vec4 texColor = texture(cubemapSampler, pTCoord);

	gl_FragData[0] = vPositionWorld;
	// Flip normal because we want the light to affect the inside of the cube
	gl_FragData[1] = vec4(-vNormal, 1.0);
	gl_FragData[2] = vec4(texColor);
}
