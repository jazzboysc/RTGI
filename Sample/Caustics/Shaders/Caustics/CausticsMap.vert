#version 430

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTCoord;

out vec2 pTCoord;
out vec4 vPositionWorld;
out vec4 vNormalWorld;
out float intensity;

uniform int causticsMapResolution;
uniform sampler2D intersectionPositionSampler;

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

	vec2 coord[];

} causticsDebugBuffer;

void main()
{
	float x = gl_VertexID % 1024;
	float y = gl_VertexID / 1024;
	vec2 pTCoord = vec2(x / 1024, y / 768);
	gl_PointSize = 200.0;

    gl_Position = vec4(pTCoord, 1, 1);

	causticsDebugBuffer.value1 = gl_VertexID;
	causticsDebugBuffer.coord[gl_VertexID] = vec2(pTCoord);












	/*

	vNormalWorld = World * vec4(vNormal, 0.0);
	vPositionWorld = World * vPosition;
	pTCoord = vTCoord;


	         // look up the intersection position
	vec4 intPt = texture(intersectionPositionSampler, vTCoord);
         // set the output value
	mat4 mLightViewProj = lightProj * lightView;

	
         // compute the light intensity
	intensity = vec3(1000,1000,1000) * (4.0/causticsMapResolution);
	//intensity = intPt.xyz;

	//gl_Position = intPt * mLightViewProj;
	float a = 1.0;
	if(intPt.x < 7)
	{
		//gl_Position = vPosition;
	}
	*/
}