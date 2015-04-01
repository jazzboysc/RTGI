#version 430 core

#include "VPLviaSVOGI/sTempResult.glsl"

in vec2 pTCoord;
out vec4 Output;

uniform sampler2D tempSampler;

void main()
{
    vec4 worldPosition = texture(tempSampler, pTCoord).rgba;
    if( worldPosition.w <= 0.0 )
    {
        discard;
    }

    ivec3 res = GetIndex(worldPosition.xyz);
    int index = res.z * dim * dim + res.y * dim + res.x;
    vec4 color = UintToVec4(voxelBuffer.data[index].value1);
    color.xyz /= 255.0;
    color.w = 1.0;

    Output = color;
}