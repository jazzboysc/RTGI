#version 120

varying vec2 pTCoord;

uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D colorSampler;

void main()
{
	vec3 normal = texture2D(normalSampler, pTCoord).rgb;

	gl_FragData[0] = vec4(normal, 1.0);
}
