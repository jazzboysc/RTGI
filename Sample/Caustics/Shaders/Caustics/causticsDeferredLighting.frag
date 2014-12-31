#version 120

varying vec2 pTCoord;

uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D reflectanceSampler;

void main()
{
	vec3 worldNormal = texture2D(normalSampler, pTCoord).rgb;
	worldNormal = worldNormal.xyz*2.0 - vec3(1.0);
	vec3 worldPosition = texture2D(positionSampler, pTCoord).rgb;
	vec3 worldLightPosition = (World * vec4(lightPosition, 1.0)).xyz;

	
	vec3 lightDir = worldLightPosition - worldPosition;
	float lightDistance = length(lightDir);
	lightDir = lightDir / lightDistance;
	float cosAlpha = max(0.0, dot(lightDir, worldNormal));

	vec3 reflectance = texture2D(reflectanceSampler, pTCoord).rgb;

	vec3 diffuseColor = vec3(cosAlpha) * lightColor * reflectance;

	gl_FragData[0] = vec4(diffuseColor, 1.0);
}