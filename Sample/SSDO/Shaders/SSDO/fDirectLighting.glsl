#version 120

varying vec2 pTCoord;

uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D reflectanceSampler;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

void main()
{
	vec3 position = texture2D(positionSampler, pTCoord).rgb;

	vec3 normal = texture2D(normalSampler, pTCoord).rgb;
	normal = normal.xyz*2.0 - vec3(1.0);

	vec3 reflectance = texture2D(reflectanceSampler, pTCoord).rgb;

	vec3 ViewLightPosition = (View * World * vec4(lightPosition, 1.0)).xyz;

	vec3 lightDir = ViewLightPosition - position;
	float lightDistance = length(lightDir);
	lightDir = lightDir / lightDistance;

	float cosAlpha = max(0.0, dot(lightDir, normal));
	vec3 diffuseColor = vec3(cosAlpha) * lightColor * reflectance;

	gl_FragData[0] = vec4(diffuseColor, 1.0);
}
