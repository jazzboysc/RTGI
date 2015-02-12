#version 430

in vec2 pTCoord;
in vec4 vPositionWorld;
in vec4 vNormalWorld;
in float intensity;

uniform sampler2D intersectionPositionSampler;
uniform sampler2D causticsMapSampler;
uniform int causticsMapResolution;
uniform vec3 lightColor;

layout(std430, binding = 3)  buffer _causticsDebugBuffer
{
    // Debug.
    uint value1;
    uint value2;
    uint value3;
    uint value4;

	vec3 coord[];

} causticsDebugBuffer;

vec2 getTC(mat4 mVP, vec3 wpos)
{
	vec4 texPt = mVP * vec4(wpos, 1);
	vec2 tc = vec2(0.5*(texPt.xy / texPt.w) + vec2(0.5, 0.5));
	return tc;
}

void main()
{
	vec4 tcol = texture(causticsMapSampler, pTCoord);
	gl_FragData[0] = vec4(lightColor * intensity, 1 - tcol.r);
	//gl_FragData[0] = vec4(1, 1, 1, 1);
}