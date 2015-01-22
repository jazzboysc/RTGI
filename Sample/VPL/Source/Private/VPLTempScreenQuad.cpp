#include "VPLTempScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLTempScreenQuad::VPLTempScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
    ShowMode = 0;
    TextureArrayIndex = 0;
}
//----------------------------------------------------------------------------
VPLTempScreenQuad::~VPLTempScreenQuad()
{
	TempTexture = 0;
    TempTexture2 = 0;
    TempTextureArray = 0;
}
//----------------------------------------------------------------------------
void VPLTempScreenQuad::OnUpdateShaderConstants(int, int)
{
    mShowModeLoc.SetValue(ShowMode);
    mTextureArrayIndexLoc.SetValue(TextureArrayIndex);

    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    if( TempTexture )
    {
        mTempSamplerLoc.SetValue(0);
        TempTexture->BindToSampler(0, &sampler);
    }

    if( TempTexture2 )
    {
        mTempSampler2Loc.SetValue(1);
        TempTexture2->BindToSampler(1, &sampler);
    }

    if( TempTextureArray )
    {
        mTempSamplerArrayLoc.SetValue(2);
        TempTextureArray->BindToSampler(2, &sampler);
    }
}
//----------------------------------------------------------------------------
void VPLTempScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
    program->GetUniformLocation(&mTempSampler2Loc, "tempSampler2");
    program->GetUniformLocation(&mTempSamplerArrayLoc, "tempSamplerArray");
    program->GetUniformLocation(&mShowModeLoc, "ShowMode");
    program->GetUniformLocation(&mTextureArrayIndexLoc, "TextureArrayIndex");
}
//----------------------------------------------------------------------------