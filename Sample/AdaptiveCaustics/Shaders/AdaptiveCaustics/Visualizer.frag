#version 430 core

in vec2 pTCoord;

out vec4 Output;

uniform int ShowMode;

uniform sampler2D tempSampler;
uniform sampler2DArray tempSamplerArray;

void main()
{
    if( ShowMode == 0 ||
		ShowMode == 3)
    {
        Output = texture(tempSampler, pTCoord);
    }

	if( ShowMode == 1 ||
		ShowMode == 2)
    {
        Output = texture(tempSamplerArray, vec3(pTCoord, ShowMode - 1));
    }

}
