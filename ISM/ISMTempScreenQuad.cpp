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

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TempTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
//----------------------------------------------------------------------------
void ISMTempScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
}
//----------------------------------------------------------------------------