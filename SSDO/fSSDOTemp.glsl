#version 150

in vec2 pTCoord;
out vec4 fColor;

uniform sampler2D tempSampler;

void main()
{
	fColor = texture(tempSampler, pTCoord);
}
