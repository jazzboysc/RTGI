#version 430

in vec2 pTCoord;
in vec4 vPositionWorld;
in vec4 vNormalWorld;
in float intensity;

uniform vec3 lightPosition;
uniform mat4 lightView;
uniform mat4 lightProj;
uniform vec3 lightColor;

vec2 getTC(mat4 mVP, vec3 wpos)
{
	vec4 texPt = mVP * vec4(wpos, 1);
	vec2 tc = vec2(0.5*(texPt.xy / texPt.w) + vec2(0.5, 0.5));
	return tc;
}

void main()
{

	gl_FragData[0] = vec4(1,1,1, 1);
	//gl_FragData[0] = vec4(dist, dist, dist, 1.0) * 1.5;// vec4(intPt,1);
	////

	         // look up alpha mask

    //     float4 tcol = tex2D(pixelSampler,input.tc);
    //     return float4(input.light_int,1-tcol.r);
}