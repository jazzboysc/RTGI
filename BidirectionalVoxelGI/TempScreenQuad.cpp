#include "TempScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
TempScreenQuad::TempScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
    ShowMode = 0;
    TextureArrayIndex = 0;
}
//----------------------------------------------------------------------------
TempScreenQuad::~TempScreenQuad()
{
	TempTexture = 0;
    TempTexture2 = 0;
    TempTextureArray = 0;
}
//----------------------------------------------------------------------------
void TempScreenQuad::OnUpdateShaderConstants(int, int)
{
    GPU_DEVICE_FUNC_SetUniformValueInt(mShowModeLoc, ShowMode);
    GPU_DEVICE_FUNC_SetUniformValueInt(mTextureArrayIndexLoc, TextureArrayIndex);

    if( TempTexture )
    {
        GPU_DEVICE_FUNC_SetUniformValueInt(mTempSamplerLoc, 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TempTexture->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    if( TempTexture2 )
    {
        GPU_DEVICE_FUNC_SetUniformValueInt(mTempSampler2Loc, 1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, TempTexture2->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    if( TempTextureArray )
    {
        GPU_DEVICE_FUNC_SetUniformValueInt(mTempSamplerArrayLoc, 2);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_ARRAY, TempTextureArray->GetTexture());
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
}
//----------------------------------------------------------------------------
void TempScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
    program->GetUniformLocation(&mTempSampler2Loc, "tempSampler2");
    program->GetUniformLocation(&mTempSamplerArrayLoc, "tempSamplerArray");
    program->GetUniformLocation(&mShowModeLoc, "ShowMode");
    program->GetUniformLocation(&mTextureArrayIndexLoc, "TextureArrayIndex");
}
//----------------------------------------------------------------------------