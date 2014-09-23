#version 410 core

in vec2 pTCoord;

uniform bool UsingArray;

uniform sampler2D tempSampler;
uniform sampler2DArray tempSamplerArray;

void main()
{
    if( !UsingArray )
    {
        gl_FragData[0] = texture(tempSamplerArray, vec3(pTCoord, 2));
    }
    else
    {
        gl_FragData[0] = texture(tempSampler, pTCoord);
    }
}