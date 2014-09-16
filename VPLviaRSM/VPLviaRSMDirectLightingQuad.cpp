#include "VPLviaRSMDirectLightingQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLviaRSMDirectLightingQuad::VPLviaRSMDirectLightingQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
    LightProjector = 0;
}
//----------------------------------------------------------------------------
VPLviaRSMDirectLightingQuad::~VPLviaRSMDirectLightingQuad()
{
    GBufferPositionTexture = 0;
    GBufferNormalTexture = 0;
    GBufferDiffuseTexture = 0;
	ShadowMap = 0;
}
//----------------------------------------------------------------------------
void VPLviaRSMDirectLightingQuad::OnUpdateShaderConstants(int, int)
{
    assert( LightProjector );
    if( LightProjector )
    {
        mat4 viewTrans = LightProjector->GetViewTransform();
        glUniformMatrix4fv(mLightProjectorViewLoc, 1, GL_TRUE, viewTrans);

        vec3 lightPosition = LightProjector->GetLocation();
        glUniform3fv(mLightPositionWorldLoc, 1, (GLfloat*)&lightPosition);

        float nearFarPlane[2];
        LightProjector->GetNearFarPlane(nearFarPlane);
        glUniform2fv(mLightProjectorNearFarLoc, 1, nearFarPlane);
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
    glBindTexture(GL_TEXTURE_2D, ShadowMap->GetTexture());
	glUniform1i(mShadowMapSamplerLoc, 3);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
}
//----------------------------------------------------------------------------
void VPLviaRSMDirectLightingQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();

    mLightProjectorViewLoc = 
        glGetUniformLocation(program, "LightProjectorView");
    mLightPositionWorldLoc = 
        glGetUniformLocation(program, "LightPositionWorld");
    mLightColorLoc = glGetUniformLocation(program, "LightColor");
    mLightProjectorNearFarLoc = 
        glGetUniformLocation(program, "LightProjectorNearFar");

    mGBufferPositionSamplerLoc = 
        glGetUniformLocation(program, "GBufferPositionSampler");
    mGBufferNormalSamplerLoc = 
        glGetUniformLocation(program, "GBufferNormalSampler");
    mGBufferDiffuseSamplerLoc = 
        glGetUniformLocation(program, "GBufferDiffuseSampler");
    mShadowMapSamplerLoc = glGetUniformLocation(program, "ShadowMapSampler");
}
//----------------------------------------------------------------------------