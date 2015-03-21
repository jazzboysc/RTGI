#version 430 core

#include "VPLviaSVOGI/sVirtualPointLight.glsl"

in vec4 vPosition;
out vec4 Color;

uniform mat4 View;
uniform mat4 Proj;

uniform int CurVPLSubsetID;
uniform int VPLCount;
uniform int PatternSize;
uniform bool ShowVPLSubset;
uniform bool ShowVPLFluxContrast;

void main()
{
    vec4 worldPosition;
    vec4 vplFlux;
    if( ShowVPLSubset )
    {
        int subsetSize = VPLCount / (PatternSize*PatternSize);
        int baseIndex = subsetSize*CurVPLSubsetID;
        worldPosition = VPLBuffer.vpls[baseIndex + gl_InstanceID].WorldPosition;
        vplFlux = VPLBuffer.vpls[baseIndex + gl_InstanceID].Flux;
    }
    else
    {
        worldPosition = VPLBuffer.vpls[gl_InstanceID].WorldPosition;
        vplFlux = VPLBuffer.vpls[gl_InstanceID].Flux;
    }

    if( ShowVPLFluxContrast )
    {
        float contrast = length(vplFlux.rrg - vplFlux.gbb) / (sqrt(2.0) + vplFlux.r + vplFlux.g + vplFlux.b);
        Color = vec4(contrast, contrast, contrast, 1.0);
    }
    else
    {
        Color = vec4(1.0, 1.0, 1.0, 1.0);
    }

    gl_Position =  Proj * View * worldPosition;
}