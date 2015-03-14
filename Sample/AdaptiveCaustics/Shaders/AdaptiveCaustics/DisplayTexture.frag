#version 430 core

in vec2 pTCoord;

out vec4 Output;

uniform int ShowMode;

uniform sampler2D tempSampler;
uniform sampler2D tempSampler2;

void main()
{
    if( ShowMode == 0 ||
		ShowMode == 1 ||
		ShowMode == 2)
    {
        Output = texture(tempSampler, pTCoord);
    }
}
