#version 120

varying vec2 pTCoord;
varying vec4 vPositionWorld;
varying vec4 vNormalWorld;

uniform vec3 lightPosition;
uniform mat4 lightView;
uniform mat4 lightProj;
uniform vec3 lightColor;

uniform float refractionIndex;

uniform sampler2D positionSampler;
uniform sampler2D normalSampler;
uniform sampler2D reflectanceSampler;
uniform sampler2D vertexSplattingSampler;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

vec2 getTC(mat4 mVP, vec3 wpos)
{
	vec4 texPt = mVP * vec4(wpos, 1);
	vec2 tc = vec2(0.5*(texPt.xy / texPt.w) + vec2(0.5, 0.5));

    tc.y = 1.0f - tc.y;

	return tc;
}

vec3 EstimateIntersection (vec3 v, vec3 r, sampler2D posTexture)
{
	vec3 P1 = v + 1.0 * r;
	vec4 texPt = Proj * View * vec4(P1, 1);
	vec2 tc = vec2(0.5) * texPt.xy + vec2(0.5, 0.5);
	tc.y = 1.0f - tc.y;
	vec4 recPos = texture2D(posTexture, tc);
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
		vec3 newPos1 = texture2D(posTex, tc).xyz;
		float f_x_k = distance(newPos1 , pos_p);
		
		// f(x_k + eps)
		vec3 pos_q = pos + dir * (x_k + eps);
		tc = getTC(mVP, pos_q);
		vec3 newPos2 = texture2D(posTex, tc).xyz;
		float f_x_k_eps = distance(newPos2, pos_q);
		float deriv = (f_x_k_eps - f_x_k / eps);
		
		// the newton raphson iteration
		x_k = x_k - (f_x_k / deriv);
	}

	return x_k;
}

void mainmain()
{
	vec3 posi = vPositionWorld.xyz;
	vec3 normal = normalize(vNormalWorld).xyz;
	mat4 mVP = lightProj * lightView;
	// Compute refracted ray direction
	vec3 lightDir = normalize(posi - lightPosition);
	vec3 refrDir = normalize(refract(lightDir, normal, refractionIndex));

	// Get distance to ray/geometry intersection estimation point
	float dist = rayGeoNP(posi, refrDir, mVP, positionSampler, 1);

	// Compute the intersection point
	vec3 intPt = posi + refrDir*dist;

	//return vec4(intPt,1);
}

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
	
	//gl_FragData[0] = vec4(diffuseColor, 1.0);
	
	////
	vec3 posi = vPositionWorld.xyz;
	vec3 normal = normalize(vNormalWorld).xyz;
	mat4 mVP = lightProj * lightView;
	// Compute refracted ray direction
	//vec3 lightDir = normalize(posi - lightPosition);
	vec3 refrDir = normalize(refract(-lightDir, normal, refractionIndex));

	// Get distance to ray/geometry intersection estimation point
	float dist = rayGeoNP(posi, refrDir, mVP, positionSampler, 1);

	// Compute the intersection point
	vec3 intPt = posi + refrDir * dist;

	gl_FragData[0] = vec4(diffuseColor, 1.0) * vec4(intPt,1);
	////


}