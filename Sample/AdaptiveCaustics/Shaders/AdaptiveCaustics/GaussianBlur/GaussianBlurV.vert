#version 430

in vec4 vPosition;
in vec3 vNormal;
in vec2 vTCoord;

out vec2 pTCoord;

uniform int causticsMapResolution;
uniform sampler2D intersectionPositionSampler;
out float texelWidthOffset;
out float texelHeightOffset;
out vec2 blurCoordinates[5];


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
	texelWidthOffset = 1 / 1024;
	texelHeightOffset = 1 / 768;

	pTCoord = vTCoord;
	gl_Position = vPosition;
 
	vec2 singleStepOffset = vec2(texelWidthOffset, texelHeightOffset);
	blurCoordinates[0] = vTCoord;
	blurCoordinates[1] = vTCoord + singleStepOffset * 1.407333;
	blurCoordinates[2] = vTCoord - singleStepOffset * 1.407333;
	blurCoordinates[3] = vTCoord + singleStepOffset * 3.294215;
	blurCoordinates[4] = vTCoord - singleStepOffset * 3.294215;
}