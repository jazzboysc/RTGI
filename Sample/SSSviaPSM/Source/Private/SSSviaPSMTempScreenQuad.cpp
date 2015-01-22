#include "SSSviaPSMTempScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SSSviaPSMTempScreenQuad::SSSviaPSMTempScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
}
//----------------------------------------------------------------------------
SSSviaPSMTempScreenQuad::~SSSviaPSMTempScreenQuad()
{
	TempTexture = 0;
}
//----------------------------------------------------------------------------
void SSSviaPSMTempScreenQuad::OnUpdateShaderConstants(int, int)
{
    int samplerIndex = 0;
    mTempSamplerLoc.SetValue(samplerIndex);

    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    TempTexture->BindToSampler(samplerIndex, &sampler);
}
//----------------------------------------------------------------------------
void SSSviaPSMTempScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
}
//----------------------------------------------------------------------------