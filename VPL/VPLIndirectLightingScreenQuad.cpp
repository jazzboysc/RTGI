#include "VPLIndirectLightingScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLIndirectLightingScreenQuad::VPLIndirectLightingScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
}
//----------------------------------------------------------------------------
VPLIndirectLightingScreenQuad::~VPLIndirectLightingScreenQuad()
{
    GBufferPositionTexture = 0;
    GBufferNormalTexture = 0;
    GBufferAlbedoTexture = 0;
    VPLBuffer = 0;
}
//----------------------------------------------------------------------------
void VPLIndirectLightingScreenQuad::OnUpdateShaderConstants(int, int)
{
    glUniform1i(mVPLCountLoc, VPLCount);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GBufferPositionTexture->GetTexture());
    glUniform1i(mGBufferPositionSamplerLoc, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, GBufferNormalTexture->GetTexture());
    glUniform1i(mGBufferNormalSamplerLoc, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, GBufferAlbedoTexture->GetTexture());
    glUniform1i(mGBufferAlbedoSamplerLoc, 2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    VPLBuffer->Bind(0);
}
//----------------------------------------------------------------------------
void VPLIndirectLightingScreenQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();

    mVPLCountLoc = glGetUniformLocation(program, "VPLCount");
    mGBufferPositionSamplerLoc = glGetUniformLocation(program, "GBufferPositionSampler");
    mGBufferNormalSamplerLoc = glGetUniformLocation(program, "GBufferNormalSampler");
    mGBufferAlbedoSamplerLoc = glGetUniformLocation(program, "GBufferAlbedoSampler");
}
//----------------------------------------------------------------------------