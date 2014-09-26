#include "VPLTempScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLTempScreenQuad::VPLTempScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
    ShowMode = 0;
    RSMFaceIndex = 0;
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
    glUniform1i(mShowModeLoc, ShowMode);
    glUniform1i(mRSMFaceIndexLoc, RSMFaceIndex);

    if( TempTexture )
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TempTexture->GetTexture());
        glUniform1i(mTempSamplerLoc, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    if( TempTexture2 )
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, TempTexture2->GetTexture());
        glUniform1i(mTempSampler2Loc, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    if( TempTextureArray )
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_ARRAY, TempTextureArray->GetTexture());
        glUniform1i(mTempSamplerArrayLoc, 2);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }
}
//----------------------------------------------------------------------------
void VPLTempScreenQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mTempSamplerLoc = glGetUniformLocation(program, "tempSampler");
    mTempSampler2Loc = glGetUniformLocation(program, "tempSampler2");
    mTempSamplerArrayLoc = glGetUniformLocation(program, "tempSamplerArray");
    mShowModeLoc = glGetUniformLocation(program, "ShowMode");
    mRSMFaceIndexLoc = glGetUniformLocation(program, "RSMFaceIndex");
}
//----------------------------------------------------------------------------