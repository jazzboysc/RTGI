#version 410 core

in vec2 pTCoord;

uniform bool UsingArray;
uniform int RSMFaceIndex;

uniform sampler2D tempSampler;
uniform sampler2DArray tempSamplerArray;

void main()
{
    if( UsingArray )
    {
        gl_FragData[0] = texture(tempSamplerArray, vec3(pTCoord, RSMFaceIndex));
    }
    else
    {
        gl_FragData[0] = texture(tempSampler, pTCoord);
    }
}
