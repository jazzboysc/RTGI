#version 430

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTCoord;

out vec2 pTCoord;
out vec4 vPositionWorld;
out vec4 vNormalWorld;
out float intensity;

uniform vec3 lightPosition;
uniform mat4 lightView;
uniform mat4 lightProj;
uniform vec3 lightColor;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;


uniform float causticsMapResolution;
uniform sampler2D intersectionPositionSampler;

void main()
{
	//*
    gl_Position = vPosition;
	gl_Position.w = 1.0;

	vNormalWorld = World * vec4(vNormal, 0.0);
	vPositionWorld = World * vPosition;
	pTCoord = vTCoord;


	         // look up the intersection position
	vec4 intPt = texture(intersectionPositionSampler, vTCoord);
         // set the output value
	mat4 mLightViewProj = lightProj * lightView;

	
         // compute the light intensity
	intensity = vec3(1000,1000,1000)*(4.0/causticsMapResolution);

	gl_Position = intPt * mLightViewProj;

	//*/
}