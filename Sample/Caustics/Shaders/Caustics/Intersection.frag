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

	vec4 recPos = texture(posTexture, tc);
	vec3 P2 = v + distance(v, recPos.xyz) * r;
	texPt = Proj * View * vec4(P2, 1);
	tc = 0.5 * texPt.xy / texPt.w + vec2(0.5, 0.5);
	return texture2D(posTexture, tc).xyz;
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
		tc = getTC(mVP, pos_p);
		vec3 newPos2 = texture(posTex, tc).xyz;
		float f_x_k_eps = distance(newPos2, pos_q);

		float deriv = (f_x_k_eps - f_x_k) / eps;
		
		// the newton raphson iteration
		x_k = x_k - (f_x_k / deriv);
	}

	return x_k;
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
	if (lightRefracterPos.rgb == vec3(0.0,0.0,0.0))
	{
		//discard;
	}



	vec3 lightRefracterNorm = texture(refracterNormalLightSampler, pTCoord).xyz;
	//lightRefracterNorm = lightRefracterNorm * 0.5 + vec3(0.5);

	vec3 lightDir = normalize( lightReceiverPos - lightPosition);// normalize( (vec4(lightReceiverPos, 1)).xyz - (vec4(lightPosition,1)).xyz);
	// Compute refracted ray direction
	//vec3 lightDir = normalize(posi - lightPosition);
	vec3 refracDir = normalize(refract(lightDir, lightRefracterNorm, 0.95));

	// Get distance to ray/geometry intersection estimation point
	float dist = rayGeoNP(lightRefracterPos, refracDir, mVPLight, receiverPositionLightSampler, 32);

	/*
	float eps = 0.1;
	vec2 tc = vec2(0.0, 0.0);

	// initial guess
	float x_k = 0.10;

	for(int i = 0; i < 30; i++)
	{
		// f(x_k)
		vec3 pos_p = 0.0 + vec3(0, -1, 0) * x_k; //lightRefracterPos + refracDir * x_k;
		pos_p = (vec4(pos_p, 1) * mVPLight).xyz;
		tc = vec2(0.5* pos_p.xy + vec2(0.5, 0.5));
		vec3 newPos1 = texture(receiverPositionLightSampler, tc).xyz;
		float f_x_k = distance(newPos1 , pos_p);
		
		// f(x_k + eps)
		vec3 pos_q = 0.0 + vec3(0, -1, 0) * (x_k + eps);//lightRefracterPos + refracDir * (x_k + eps);
		pos_p = (vec4(pos_p, 1) * mVPLight).xyz;
		tc = vec2(0.5* pos_p.xy + vec2(0.5, 0.5));
		vec3 newPos2 = texture(receiverPositionLightSampler, tc).xyz;
		float f_x_k_eps = distance(newPos2, pos_q);

		float deriv = (f_x_k_eps - f_x_k) / eps;
		// the newton raphson iteration
		x_k = x_k - (f_x_k / deriv);
	}

	float dist = x_k;
	//*/

	//float dist = EstimateIntersection(lightRefracterPos, refracDir, receiverPositionLightSampler);
	// Compute the intersection point
	vec3 intPt = lightRefracterPos + refracDir * dist;
	vec3 intPtLight = (vec4(intPt, 1) * mVPLight).xyz;
	vec3 texColor = texture(cubeSampler, refracDir).xyz;

	gl_FragData[0] = vec4(intPt, 1);
	float aa = 0;
	if(intPt.y < -1)
	{
		//gl_FragData[0] = vec4(1, 1, 1, 1);
	}
	// Testing
	gl_FragData[0] = vec4(texColor, 1);
	//gl_FragData[0] = vec4(dist, dist, dist, 1.0);// * 4 - 3.0f;// vec4(intPt,1);
	//gl_FragData[0] = vec4(intPt, 1);
	////


}