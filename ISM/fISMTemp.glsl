#version 430

in vec2 pTCoord;

uniform sampler2D tempSampler;

void main()
{
    gl_FragData[0] = texture(tempSampler, pTCoord);
}
