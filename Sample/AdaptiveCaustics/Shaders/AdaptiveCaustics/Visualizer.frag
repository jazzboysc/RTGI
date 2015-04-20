#version 430 core

in vec2 pTCoord;

out vec4 Output;

uniform int ShowMode;

uniform sampler2D tempSampler;
uniform sampler2DArray tempSamplerArray;

void main()
{
    if(	ShowMode == 2 ||
		ShowMode == 7 ||
		ShowMode == 8 ||
		ShowMode == 9 ||
		ShowMode == 10)
    {
        Output = texture(tempSampler, pTCoord);
    }

	if( ShowMode == 0 ||
		ShowMode == 1 || // Normal
		ShowMode == 3)
	{
        Output = texture(tempSampler, pTCoord);
		Output = Output / 2 + vec4(0.5);
	}

	if( ShowMode == 6)
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

	if( ShowMode == 4 ||
		ShowMode == 5)
    {
        Output = texture(tempSamplerArray, vec3(pTCoord, ShowMode - 4));
		Output = Output / 2 + vec4(0.5);
    }
}
