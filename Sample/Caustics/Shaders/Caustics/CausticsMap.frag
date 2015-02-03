#version 430

in vec2 pTCoord;
in vec4 vPositionWorld;
in vec4 vNormalWorld;
in float intensity;

uniform int causticsMapResolution;
uniform sampler2D intersectionPositionSampler;
uniform sampler2D causticsMapSampler;

uniform vec3 lightPosition;
uniform mat4 lightView;
uniform mat4 lightProj;
uniform vec3 lightColor;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

vec2 getTC(mat4 mVP, vec3 wpos)
{
	vec4 texPt = mVP * vec4(wpos, 1);
	vec2 tc = vec2(0.5*(texPt.xy / texPt.w) + vec2(0.5, 0.5));
	return tc;
}

void main()
{

	float inten = intensity; //vec3(1000,1000,1000)*(4.0/10000);
	mat4 ab = World;
	float a = causticsMapResolution;
	vec3 intPt = texture(intersectionPositionSampler, pTCoord).xyz;

	if (intPt.rgb == vec3(0.0,0.0,0.0))
		discard;


	vec2 pTCoord = vec2(0.5 * (vPositionWorld.xy / vPositionWorld.w) + vec2(0.5, 0.5));
	vec4 tcol = texture(causticsMapSampler, pTCoord);
	gl_FragData[0] = vec4(inten, inten, inten, 1 - tcol.r);
	gl_FragData[0] = vec4(1, 1, 1, 1);
}