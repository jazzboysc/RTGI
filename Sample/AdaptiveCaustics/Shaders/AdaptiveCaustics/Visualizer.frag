#version 430 core

in vec2 pTCoord;

out vec4 Output;

uniform int ShowMode;

uniform sampler2D tempSampler;
uniform sampler2DArray tempSamplerArray;

void main()
{
    if( ShowMode == 0)
    {
        Output = texture(tempSampler, pTCoord);
    }

	if( ShowMode == 3)
    {
		vec4 color = texture(tempSampler, pTCoord);
		if( color.a != 0)
		{
			Output = vec4(color.r * 1.5, color.g * 1.5, color.b * 1.5, color.a);
		}
		else
		{
			Output = vec4(0,0,0,0);
		}
    }

	if( ShowMode == 1 ||
		ShowMode == 2)
    {
        Output = texture(tempSamplerArray, vec3(pTCoord, ShowMode - 1));
    }

}
