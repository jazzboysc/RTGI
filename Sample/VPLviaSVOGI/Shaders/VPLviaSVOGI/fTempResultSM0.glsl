#version 430 core

#include "VPLviaSVOGI/sTempResult.glsl"

in vec2 pTCoord;
out vec4 Output;

uniform sampler2D tempSampler;

void main()
{
    Output = texture(tempSampler, pTCoord);
}