#include "SSDOTempScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SSDOTempScreenQuad::SSDOTempScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
}
//----------------------------------------------------------------------------
SSDOTempScreenQuad::~SSDOTempScreenQuad()
{
	TempTexture = 0;
}
//----------------------------------------------------------------------------
void SSDOTempScreenQuad::OnUpdateShaderConstants(int, int)
{
    mTempSamplerLoc.SetValue(0);

    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    TempTexture->BindToSampler(0, &sampler);
}
//----------------------------------------------------------------------------
void SSDOTempScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
}
//----------------------------------------------------------------------------