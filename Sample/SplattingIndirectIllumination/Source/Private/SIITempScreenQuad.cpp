#include "SIITempScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SIITempScreenQuad::SIITempScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
}
//----------------------------------------------------------------------------
SIITempScreenQuad::~SIITempScreenQuad()
{
	TempTexture = 0;
}
//----------------------------------------------------------------------------
void SIITempScreenQuad::OnUpdateShaderConstants(int, int)
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
void SIITempScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
}
//----------------------------------------------------------------------------