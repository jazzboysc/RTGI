#include "SSDOFilterScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SSDOFilterScreenQuad::SSDOFilterScreenQuad(Material* material)
	:
	ScreenQuad(material)
{
}
//----------------------------------------------------------------------------
SSDOFilterScreenQuad::~SSDOFilterScreenQuad()
{
	SSDOTexture = 0;
	PositionTexture = 0;
	NormalTexture = 0;
	ColorTexture = 0;
	DirectLightingTexture = 0;
}
//----------------------------------------------------------------------------
void SSDOFilterScreenQuad::OnUpdateShaderConstants(int, int)
{
    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    SSDOTexture->BindToSampler(0, &sampler);
    PositionTexture->BindToSampler(1, &sampler);
    NormalTexture->BindToSampler(2, &sampler);
    ColorTexture->BindToSampler(3, &sampler);
    DirectLightingTexture->BindToSampler(4, &sampler);

    mSSDOSamplerLoc.SetValue(0);
    mPositionSamplerLoc.SetValue(1);
    mNormalSamplerLoc.SetValue(2);
    mColorSamplerLoc.SetValue(3);
    mDirectLightingSamplerLoc.SetValue(4);
    mPositionThresholdLoc.SetValue(PositionThreshold);
    mNormalThresholdLoc.SetValue(NormalThreshold);
    mMaxRadianceLoc.SetValue(MaxRadiance);
    mKernelSizeLoc.SetValue(KernelSize);
}
//----------------------------------------------------------------------------
void SSDOFilterScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mSSDOSamplerLoc, "ssdoSampler");
    program->GetUniformLocation(&mPositionSamplerLoc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerLoc, "normalSampler");
    program->GetUniformLocation(&mColorSamplerLoc, "colorSampler");
    program->GetUniformLocation(&mDirectLightingSamplerLoc, "directLightingSampler");
    program->GetUniformLocation(&mPositionThresholdLoc, "positionThreshold");
    program->GetUniformLocation(&mNormalThresholdLoc, "normalThreshold");
    program->GetUniformLocation(&mMaxRadianceLoc, "maxRadiance");
    program->GetUniformLocation(&mKernelSizeLoc, "kernelSize");
}
//----------------------------------------------------------------------------