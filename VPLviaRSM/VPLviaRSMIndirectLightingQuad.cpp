#include "VPLviaRSMIndirectLightingQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLviaRSMIndirectLightingQuad::VPLviaRSMIndirectLightingQuad(
    Material* material)
	:
	ScreenQuad(material, 0),
    VPLColor(0.0f)
{
    VPLProjector = 0;
}
//----------------------------------------------------------------------------
VPLviaRSMIndirectLightingQuad::~VPLviaRSMIndirectLightingQuad()
{
    GBufferPositionTexture = 0;
    GBufferNormalTexture = 0;
    GBufferDiffuseTexture = 0;
	VPLShadowMap = 0;
}
//----------------------------------------------------------------------------
void VPLviaRSMIndirectLightingQuad::OnUpdateShaderConstants(int, int)
{
    assert( VPLProjector );
    if( VPLProjector )
    {
        mat4 viewTrans = VPLProjector->GetViewTransform();
        glUniformMatrix4fv(mVPLProjectorViewLoc, 1, GL_TRUE, viewTrans);

        vec3 vplPosition = VPLProjector->GetLocation();
        glUniform3fv(mVPLPositionWorldLoc, 1, (GLfloat*)&vplPosition);

        vec3 vplNormal = -VPLProjector->GetDirection();
        glUniform3fv(mVPLNormalWorldLoc, 1, (GLfloat*)&vplNormal);

        glUniform3fv(mVPLColorLoc, 1, (GLfloat*)&VPLColor);

        float nearFarPlane[2];
        VPLProjector->GetNearFarPlane(nearFarPlane);
        glUniform2fv(mVPLProjectorNearFarLoc, 1, nearFarPlane);
    }

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
    glBindTexture(GL_TEXTURE_2D, GBufferDiffuseTexture->GetTexture());
    glUniform1i(mGBufferDiffuseSamplerLoc, 2);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, VPLShadowMap->GetTexture());
	glUniform1i(mVPLShadowMapSamplerLoc, 3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
//----------------------------------------------------------------------------
void VPLviaRSMIndirectLightingQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();

    mVPLProjectorViewLoc = 
        glGetUniformLocation(program, "VPLProjectorView");
    mVPLPositionWorldLoc = 
        glGetUniformLocation(program, "VPLPositionWorld");
    mVPLNormalWorldLoc =
        glGetUniformLocation(program, "VPLNormalWorld");
    mVPLColorLoc = glGetUniformLocation(program, "VPLColor");
    mVPLProjectorNearFarLoc = 
        glGetUniformLocation(program, "VPLProjectorNearFar");

    mGBufferPositionSamplerLoc = 
        glGetUniformLocation(program, "GBufferPositionSampler");
    mGBufferNormalSamplerLoc = 
        glGetUniformLocation(program, "GBufferNormalSampler");
    mGBufferDiffuseSamplerLoc = 
        glGetUniformLocation(program, "GBufferDiffuseSampler");
    mVPLShadowMapSamplerLoc = 
        glGetUniformLocation(program, "VPLShadowMapSampler");
}
//----------------------------------------------------------------------------