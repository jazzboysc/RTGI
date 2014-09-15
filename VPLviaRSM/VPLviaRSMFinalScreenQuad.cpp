#include "VPLviaRSMFinalScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLviaRSMFinalScreenQuad::VPLviaRSMFinalScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
}
//----------------------------------------------------------------------------
VPLviaRSMFinalScreenQuad::~VPLviaRSMFinalScreenQuad()
{
	DirectLightingTexture = 0;
    IndirectLightingTexture = 0;
}
//----------------------------------------------------------------------------
void VPLviaRSMFinalScreenQuad::OnUpdateShaderConstants(int, int)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, DirectLightingTexture->GetTexture());
    glUniform1i(mDirectLightingSamplerLoc, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, IndirectLightingTexture->GetTexture());
    glUniform1i(mIndirectLightingSamplerLoc, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
//----------------------------------------------------------------------------
void VPLviaRSMFinalScreenQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
    mDirectLightingSamplerLoc = 
        glGetUniformLocation(program, "DirectLightingSampler");
    mIndirectLightingSamplerLoc =
        glGetUniformLocation(program, "IndirectLightingSampler");
}
//----------------------------------------------------------------------------