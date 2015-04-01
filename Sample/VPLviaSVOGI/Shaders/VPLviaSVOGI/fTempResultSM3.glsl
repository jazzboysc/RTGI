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

    vec3 normalizedPosition = (worldPosition.xyz - SceneBBMin) / (2.0*SceneBBExtension);
    Output = vec4(normalizedPosition, 1.0);
}