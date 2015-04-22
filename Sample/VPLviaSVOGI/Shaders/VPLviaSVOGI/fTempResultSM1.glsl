#version 430 core

#include "VPLviaSVOGI/sTempResult.glsl"

in vec2 pTCoord;
out vec4 Output;

uniform int TextureArrayIndex;
uniform sampler2DArray tempSamplerArray;
uniform bool IsShadowMap;

void main()
{
    if( IsShadowMap )
    {
        float depth = texture(tempSamplerArray, vec3(pTCoord, TextureArrayIndex)).r;
        Output = vec4(depth, depth, depth, 1.0);
    }
    else
    {
        Output = texture(tempSamplerArray, vec3(pTCoord, TextureArrayIndex));
    }
}