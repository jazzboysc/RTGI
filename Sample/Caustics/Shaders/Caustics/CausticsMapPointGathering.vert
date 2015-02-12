#version 430

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTCoord;

out vec2 pTCoord;
out float intensity;

uniform sampler2D causticsMapSampler;
uniform sampler2D intersectionPositionSampler;
uniform int causticsMapResolution;
uniform mat4 lightView;
uniform mat4 lightProj;

layout(std430, binding = 3)  buffer _causticsDebugBuffer
{
    // Debug.
    uint value1;
    uint value2;
    uint value3;
    uint value4;

	vec3 coord[];

} causticsDebugBuffer;

void main()
{
	mat4 mLightViewProj = lightProj * lightView;
	float x = gl_VertexID % 1024;
	float y = gl_VertexID / 1024;
	pTCoord = vec2(x / 1024, y / 768);
	vec2 tc = pTCoord * 2 - 1;

	vec3 intPt = texture(intersectionPositionSampler, pTCoord).xyz;
	
	vec3 finalPt = (mLightViewProj * vec4(intPt, 1)).xyz;
    gl_Position = vec4(finalPt, 1);

	causticsDebugBuffer.value1 = gl_VertexID;
	causticsDebugBuffer.coord[gl_VertexID] = finalPt;

	intensity = vec3(1000,1000,1000) * (4.0 / causticsMapResolution);











	/*

	vNormalWorld = World * vec4(vNormal, 0.0);
	vPositionWorld = World * vPosition;
	pTCoord = vTCoord;


	         // look up the intersection position
	vec4 intPt = texture(intersectionPositionSampler, vTCoord);
         // set the output value
	mat4 mLightViewProj = lightProj * lightView;

	
         // compute the light intensity
	//intensity = intPt.xyz;

	//gl_Position = intPt * mLightViewProj;
	float a = 1.0;
	if(intPt.x < 7)
	{
		//gl_Position = vPosition;
	}
	*/
}