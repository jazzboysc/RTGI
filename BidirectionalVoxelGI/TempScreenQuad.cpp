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
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TempTexture->GetTexture());
        GPU_DEVICE_FUNC_SetUniformValueInt(mTempSamplerLoc, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    if( TempTexture2 )
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, TempTexture2->GetTexture());
        GPU_DEVICE_FUNC_SetUniformValueInt(mTempSampler2Loc, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    if( TempTextureArray )
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_ARRAY, TempTextureArray->GetTexture());
        GPU_DEVICE_FUNC_SetUniformValueInt(mTempSamplerArrayLoc, 2);
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
    GPU_DEVICE_FUNC_GetUniformLocation(program, mTempSamplerLoc, "tempSampler");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mTempSampler2Loc, "tempSampler2");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mTempSamplerArrayLoc, "tempSamplerArray");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mShowModeLoc, "ShowMode");
    GPU_DEVICE_FUNC_GetUniformLocation(program, mTextureArrayIndexLoc, "TextureArrayIndex");
}
//----------------------------------------------------------------------------