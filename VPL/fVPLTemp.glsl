#version 410 core

in vec2 pTCoord;

uniform sampler2D tempSampler;

void main()
{
    gl_FragData[0] = texture(tempSampler, pTCoord);
}
