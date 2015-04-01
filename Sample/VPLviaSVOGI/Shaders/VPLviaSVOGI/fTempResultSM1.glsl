#version 430 core

#include "VPLviaSVOGI/sTempResult.glsl"

in vec2 pTCoord;
out vec4 Output;

uniform int TextureArrayIndex;
uniform sampler2DArray tempSamplerArray;

void main()
{
    Output = texture(tempSamplerArray, vec3(pTCoord, TextureArrayIndex));
}