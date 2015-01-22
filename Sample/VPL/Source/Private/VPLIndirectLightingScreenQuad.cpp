#include "VPLIndirectLightingScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLIndirectLightingScreenQuad::VPLIndirectLightingScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
    BounceSingularity = 60.0f;
}
//----------------------------------------------------------------------------
VPLIndirectLightingScreenQuad::~VPLIndirectLightingScreenQuad()
{
    GBufferPositionTexture = 0;
    GBufferNormalTexture = 0;
    GBufferAlbedoTexture = 0;
    VPLBuffer = 0;
}
//----------------------------------------------------------------------------
void VPLIndirectLightingScreenQuad::OnUpdateShaderConstants(int, int)
{
    mVPLCountLoc.SetValue(VPLCount);
    mBounceSingularityLoc.SetValue(BounceSingularity);

    mGBufferPositionSamplerLoc.SetValue(0);
    mGBufferNormalSamplerLoc.SetValue(1);
    mGBufferAlbedoSamplerLoc.SetValue(2);

    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    GBufferPositionTexture->BindToSampler(0, &sampler);
    GBufferNormalTexture->BindToSampler(1, &sampler);
    GBufferAlbedoTexture->BindToSampler(2, &sampler);

    VPLBuffer->Bind(0);
}
//----------------------------------------------------------------------------
void VPLIndirectLightingScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);

    program->GetUniformLocation(&mVPLCountLoc, "VPLCount");
    program->GetUniformLocation(&mBounceSingularityLoc, "BounceSingularity");
    program->GetUniformLocation(&mGBufferPositionSamplerLoc, "GBufferPositionSampler");
    program->GetUniformLocation(&mGBufferNormalSamplerLoc, "GBufferNormalSampler");
    program->GetUniformLocation(&mGBufferAlbedoSamplerLoc, "GBufferAlbedoSampler");
}
//----------------------------------------------------------------------------