#version 430

in vec2 pTCoord;
in vec4 vPositionWorld;
in vec4 vNormalWorld;

uniform vec3 lightPosition;
uniform mat4 lightView;
uniform mat4 lightProj;
uniform vec3 lightColor;

uniform float refractionIndex;

uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D reflectanceSampler;
uniform sampler2D refracterPositionLightSampler;
uniform sampler2D refracterNormalLightSampler;
uniform sampler2D receiverPositionLightSampler;

uniform samplerCube cubeSampler;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

vec2 getTC(mat4 mVP, vec3 wpos)
{
	vec4 texPt = mVP * vec4(wpos, 1);
	vec2 tc = vec2(0.5*(texPt.xy / texPt.w) + vec2(0.5, 0.5));
	return tc;
}

vec3 EstimateIntersection (vec3 v, vec3 r, sampler2D posTexture)
{
	vec3 P1 = v + 1.0 * r;
	vec4 texPt = Proj * View * vec4(P1, 1);
	vec2 tc = vec2(0.5) * texPt.xy + vec2(0.5, 0.5);
	tc.y = 1.0f - tc.y;
	vec4 recPos = texture(posTexture, tc);
	vec3 P2 = v + distance(v, recPos.xyz) * r;
	texPt = Proj * View * vec4(P2, 1);
	tc = 0.5 * texPt.xy / texPt.w + vec2(0.5, 0.5);
	tc.y = 1.0f - tc.y;
	return recPos.xyz; // = texture2D(posTexture, tc);
}

float rayGeoNP(vec3 pos, vec3 dir, mat4 mVP, sampler2D posTex, int numIter)
{
	float eps = 0.1;
	vec2 tc = vec2(0.0, 0.0);

	// initial guess
	float x_k = 0.10;

	for(int i = 0; i < numIter; i++)
	{
		// f(x_k)
		vec3 pos_p = pos + dir * x_k;
		tc = getTC(mVP, pos_p);
		vec3 newPos1 = texture(posTex, tc).xyz;
		float f_x_k = distance(newPos1 , pos_p);
		
		// f(x_k + eps)
		vec3 pos_q = pos + dir * (x_k + eps);
		tc = getTC(mVP, pos_q);
		vec3 newPos2 = texture(posTex, tc).xyz;
		float f_x_k_eps = distance(newPos2, pos_q);
		float deriv = (f_x_k_eps - f_x_k / eps);
		
		// the newton raphson iteration
		x_k = x_k - (f_x_k / deriv);
	}

	return x_k;
}

void mainmain()
{
	mat4 mVPLight = lightProj * lightView;

	vec3 receiverPos = texture(receiverPositionLightSampler, getTC(mVPLight, vPositionWorld.xyz)).xyz;
	vec3 normal = normalize(vNormalWorld).xyz;
	// Compute refracted ray direction
	vec3 lightDir = normalize(receiverPos - lightPosition);
	vec3 refrDir = normalize(refract(lightDir, normal, refractionIndex));

	// Get distance to ray/geometry intersection estimation point
	float dist = rayGeoNP(receiverPos, refrDir, mVPLight, positionSampler, 1);

	// Compute the intersection point
	vec3 intPt = receiverPos + refrDir*dist;

	//return vec4(intPt,1);
}

void main()
{
	mat4 mVPLight = lightProj * lightView;
	mat4 mVPCam = Proj * View;

	vec2 pTCoord = vec2(0.5*(vPositionWorld.xy / vPositionWorld.w) + vec2(0.5, 0.5));

	vec3 receiverWorldPos = texture(positionSampler, pTCoord).xyz;
	vec3 receiverWorldCol = texture(reflectanceSampler, pTCoord).xyz;

	vec3 lightReceiverPos = texture(receiverPositionLightSampler, pTCoord).xyz;
	
	vec3 lightRefracterPos = texture(refracterPositionLightSampler, pTCoord).xyz;
	vec3 lightRefracterNorm = texture(refracterNormalLightSampler, pTCoord).xyz;
	//lightRefracterNorm = lightRefracterNorm * 0.5 + vec3(0.5);

	vec3 lightDir = normalize( lightRefracterPos - lightPosition);// normalize( (vec4(lightReceiverPos, 1)).xyz - (vec4(lightPosition,1)).xyz);
	// Compute refracted ray direction
	//vec3 lightDir = normalize(posi - lightPosition);
	vec3 refracDir = normalize(refract(lightDir, lightRefracterNorm, 0.95));

	// Get distance to ray/geometry intersection estimation point
	float dist = rayGeoNP(lightReceiverPos, refracDir, mVPLight, refracterPositionLightSampler, 5);

	// Compute the intersection point
	vec3 intPt = lightReceiverPos + refracDir * dist;

	vec3 texColor = texture(cubeSampler, refracDir).xyz;

	gl_FragData[0] = vec4(intPt, 1);
	//gl_FragData[0] = vec4(dist, dist, dist, 1.0) * 1.5;// vec4(intPt,1);
	////


}