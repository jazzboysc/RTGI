#include "VPLIndirectLightingScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLIndirectLightingScreenQuad::VPLIndirectLightingScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
    BounceSingularity = 60.0f;
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
    mVPLCountLoc.SetValue(VPLCount);
    mBounceSingularityLoc.SetValue(BounceSingularity);

    mGBufferPositionSamplerLoc.SetValue(0);
    mGBufferNormalSamplerLoc.SetValue(1);
    mGBufferAlbedoSamplerLoc.SetValue(2);

	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GBufferPositionTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, GBufferNormalTexture->GetTexture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, GBufferAlbedoTexture->GetTexture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    VPLBuffer->Bind(0);
}
//----------------------------------------------------------------------------
void VPLIndirectLightingScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);

    program->GetUniformLocation(&mVPLCountLoc, "VPLCount");
    program->GetUniformLocation(&mBounceSingularityLoc, "BounceSingularity");
    program->GetUniformLocation(&mGBufferPositionSamplerLoc, "GBufferPositionSampler");
    program->GetUniformLocation(&mGBufferNormalSamplerLoc, "GBufferNormalSampler");
    program->GetUniformLocation(&mGBufferAlbedoSamplerLoc, "GBufferAlbedoSampler");
}
//----------------------------------------------------------------------------