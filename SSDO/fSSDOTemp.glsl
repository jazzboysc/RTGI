#version 120

varying vec2 pTCoord;

uniform sampler2D tempSampler;

void main()
{
	gl_FragData[0] = texture2D(tempSampler, pTCoord);
}
