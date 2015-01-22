#include "ISMTempScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ISMTempScreenQuad::ISMTempScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
}
//----------------------------------------------------------------------------
ISMTempScreenQuad::~ISMTempScreenQuad()
{
	TempTexture = 0;
}
//----------------------------------------------------------------------------
void ISMTempScreenQuad::OnUpdateShaderConstants(int, int)
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
void ISMTempScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
}
//----------------------------------------------------------------------------