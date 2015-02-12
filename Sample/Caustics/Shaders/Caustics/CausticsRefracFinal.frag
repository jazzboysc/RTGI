#version 430

in vec2 pTCoord;
in vec4 vPositionWorld;
in vec4 vNormalWorld;
in float intensity;

uniform sampler2D refracPositionSampler;
uniform sampler2D refracNormalSampler;
uniform sampler2D refracReflectanceSampler;
uniform samplerCube cubeSampler;
uniform sampler2D shadowmapSampler;

uniform float refractionIndex;

uniform vec3 lightPosition;
uniform mat4 lightView;
uniform mat4 lightProj;
uniform vec3 lightColor;

uniform mat4 World;
uniform mat4 View;
uniform mat4 Proj;

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
	mat4 mVPLight = lightProj * lightView;
	mat4 mVPCam = Proj * View;
	vec2 pTCoord = vec2(0.5*(vPositionWorld.xy / vPositionWorld.w) + vec2(0.5, 0.5));
	//vec2 pTCoord = vec2(vPositionWorld.xy);

	vec3 worldPosition = texture(refracPositionSampler, pTCoord).rgb;
	if (worldPosition == vec3(0, 0, 0))
	{
		discard;
	}
	vec4 viewPosition = View * vec4(worldPosition, 1);
	vec4 ndcPosition = mVPCam * vec4(worldPosition, 1);
	vec4 positionLight = (mVPLight * vec4(worldPosition, 1));
	vec2 pTCoordLight = vec2(0.5*(positionLight.xy / positionLight.w) + vec2(0.5, 0.5));

	vec3 worldNormal = texture(refracNormalSampler, pTCoord).rgb;
	//worldNormal = worldNormal.xyz*2.0 - vec3(1.0);
	vec3 reflectance = texture(refracReflectanceSampler, pTCoord).rgb;

	vec3 lightDir = lightPosition - worldPosition;
	float lightDistance = length(lightDir);
	lightDir = lightDir / lightDistance;

	vec3 refracDir = refract(-lightDir, worldNormal, refractionIndex);
	vec3 environmentColor = lightColor * texture(cubeSampler, refracDir).xyz;


	float cosTheta = max(0.0, dot(lightDir, worldNormal));
	vec3 diffuseColor = vec3(cosTheta) * lightColor * reflectance;
	//diffuseColor = (vec4(diffuseColor, 1) * caustics).rgb;


	// Shadowmap
	float shadowmapDepth = texture(shadowmapSampler, pTCoordLight).r;
	float fragmentDepth = 0.5 * positionLight.z + 0.5;
	float visibility = 1.0;
	float bias = 0.005 * tan(acos(cosTheta));
	bias = clamp(bias, 0.0, 0.01);
	if ( shadowmapDepth  < fragmentDepth - bias)
	{
		visibility = 0.8;
	}

	//specular
    float specularCoefficient = 0.0;
	specularCoefficient = pow(max(0.0, dot(normalize(worldPosition - viewPosition.xyz), reflect(-lightDir, worldNormal))), 120.0);
	vec3 specular = specularCoefficient * lightColor;
    

	float ndcDepth = ndcPosition.z / ndcPosition.w;
	gl_FragDepth = ((gl_DepthRange.diff * ndcDepth) + gl_DepthRange.near + gl_DepthRange.far) / 2.0;
	//gl_FragData[0] = vec4(positionLight, 1);
	// Refracter
	gl_FragData[0] = vec4(visibility * (vec4(diffuseColor, 1) * 0.05 + vec4(environmentColor, 1)).rgb, 0.7);
	gl_FragData[0] = vec4(visibility * (mix(environmentColor, diffuseColor, 0.1) + specular), 1.0);
	//gl_FragData[0] = vec4(specular, 1);
}

/*
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
*/
