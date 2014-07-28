#include "GPGPUPostProcessingScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GPGPUPostProcessingScreenQuad::GPGPUPostProcessingScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
}
//----------------------------------------------------------------------------
GPGPUPostProcessingScreenQuad::~GPGPUPostProcessingScreenQuad()
{
	RenderTexture = 0;
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingScreenQuad::OnUpdateShaderConstants(int, int)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, RenderTexture->GetTexture());
	glUniform1i(mRenderSamplerLoc, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingScreenQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mRenderSamplerLoc = glGetUniformLocation(program, "renderSampler");
}
//----------------------------------------------------------------------------