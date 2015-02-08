#version 430

in vec2 pTCoord;
in vec4 vPositionWorld;
in vec4 vNormalWorld;
in float intensity;

uniform sampler2D blurredCausticsMapSampler;
uniform sampler2D receiverPositionSampler;
uniform sampler2D receiverNormalSampler;
uniform sampler2D receiverReflectanceSampler;

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

	vec3 worldPosition = texture(receiverPositionSampler, pTCoord).rgb;
	if (worldPosition == vec3(0, 0, 0))
	{
		//discard;
	}

	vec4 caustics = vec4(0.0);

	vec3 positionLight = (mVPLight * vec4(worldPosition, 1)).xyz;
	if (positionLight.z >= 0)
	{
		float positionLightLength = length(positionLight);
		vec2 pTCoordLight = positionLight.xy / positionLightLength;
		pTCoordLight = pTCoordLight * 0.5 + 0.5;
		caustics = texture(blurredCausticsMapSampler, pTCoordLight);
		float a = 0;
	}

	vec3 worldNormal = texture(receiverNormalSampler, pTCoord).rgb;
	//worldNormal = worldNormal.xyz*2.0 - vec3(1.0);
	vec3 reflectance = texture(receiverReflectanceSampler, pTCoord).rgb;

	vec3 lightDir = worldPosition - lightPosition;
	float lightDistance = length(lightDir);
	lightDir = lightDir / lightDistance;

	float cosAlpha = max(0.0, dot(lightDir, worldNormal));
	vec3 diffuseColor = vec3(cosAlpha) * lightColor * reflectance;
	//diffuseColor = (vec4(diffuseColor, 1) * caustics).rgb;

	gl_FragData[0] = vec4(positionLight, 1);
	gl_FragData[0] = vec4(caustics) * 0.5 + vec4(diffuseColor, 1) * 0.5;
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
