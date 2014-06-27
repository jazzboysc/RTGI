#version 120

varying vec2 pTCoord;

uniform vec3 lightPosition;
uniform vec3 lightColor;

uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D reflectanceSampler;
uniform sampler2D indirectLightingSampler;

void main()
{
	vec3 position = texture2D(positionSampler, pTCoord).rgb;
	vec3 normal = texture2D(normalSampler, pTCoord).rgb;
	vec3 reflectance = texture2D(reflectanceSampler, pTCoord).rgb;
    vec3 indirectColor = texture2D(indirectLightingSampler, pTCoord).rgb;

	vec3 lightDir = lightPosition - position;
	float lightDistance = length(lightDir);
	lightDir = lightDir / lightDistance;
	float cosAlpha = max(0.0, dot(lightDir, normal));
	vec3 directColor = cosAlpha * lightColor * reflectance;
    vec3 outColor = directColor + indirectColor;

	gl_FragData[0] = vec4(outColor, 1.0);
}
