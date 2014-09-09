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
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TempTexture->GetTexture());
	glUniform1i(mTempSamplerLoc, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
//----------------------------------------------------------------------------
void SSSviaPSMTempScreenQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mTempSamplerLoc = glGetUniformLocation(program, "tempSampler");
}
//----------------------------------------------------------------------------