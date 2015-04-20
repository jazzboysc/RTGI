#version 430

in vec2 pTCoord;
in vec4 vPositionWorld;
in vec4 vNormalWorld;

uniform sampler2D causticsMapSampler;
in highp float texelWidthOffset;
in highp float texelHeightOffset;
in highp vec2 blurCoordinates[5];

layout(std430, binding = 3)  buffer _causticsDebugBuffer
{
    // Debug.
    uint value1;
    uint value2;
    uint value3;
    uint value4;

	vec3 coord[];

} causticsDebugBuffer;

const float blurSize = 1.0/512.0;
 
void main()
{
	lowp vec4 sum = vec4(0.0);
	sum += texture(causticsMapSampler, vec2(pTCoord.x, pTCoord.y - 4.0*blurSize)) * 0.05;
	sum += texture(causticsMapSampler, vec2(pTCoord.x, pTCoord.y - 3.0*blurSize)) * 0.09;
	sum += texture(causticsMapSampler, vec2(pTCoord.x, pTCoord.y - 2.0*blurSize)) * 0.12;
	sum += texture(causticsMapSampler, vec2(pTCoord.x, pTCoord.y - blurSize)) * 0.15;
	sum += texture(causticsMapSampler, vec2(pTCoord.x, pTCoord.y)) * 0.16;
	sum += texture(causticsMapSampler, vec2(pTCoord.x, pTCoord.y + blurSize)) * 0.15;
	sum += texture(causticsMapSampler, vec2(pTCoord.x, pTCoord.y + 2.0*blurSize)) * 0.12;
	sum += texture(causticsMapSampler, vec2(pTCoord.x, pTCoord.y + 3.0*blurSize)) * 0.09;
	sum += texture(causticsMapSampler, vec2(pTCoord.x, pTCoord.y + 4.0*blurSize)) * 0.05;
 

	/*
	sum += texture(causticsMapSampler, blurCoordinates[0]) * 0.204164;
	sum += texture(causticsMapSampler, blurCoordinates[1]) * 0.304005;
	sum += texture(causticsMapSampler, blurCoordinates[2]) * 0.304005;
	sum += texture(causticsMapSampler, blurCoordinates[3]) * 0.093913;
	sum += texture(causticsMapSampler, blurCoordinates[4]) * 0.093913;
	*/
	gl_FragColor = sum;
}
