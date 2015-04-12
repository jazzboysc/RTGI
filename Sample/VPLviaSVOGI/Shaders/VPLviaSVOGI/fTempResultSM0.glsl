#version 430 core

#include "VPLviaSVOGI/sTempResult.glsl"

in vec2 pTCoord;
out vec4 Output;

uniform sampler2D tempSampler;

void main()
{
    int level = 3;
    ivec2 levelRes = textureSize(tempSampler, level);
    ivec2 pos = ivec2(pTCoord.xy * vec2(levelRes));
    Output = texelFetch(tempSampler, pos, level);
}