#include "DirectLightingScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
DirectLightingScreenQuad::DirectLightingScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
    LightColor = vec3(0.9686f, 0.9333f, 0.8392f)*0.8f;
}
//----------------------------------------------------------------------------
DirectLightingScreenQuad::~DirectLightingScreenQuad()
{
    GBufferPositionTexture = 0;
    GBufferNormalTexture = 0;
    GBufferAlbedoTexture = 0;
    ShadowMapTexture = 0;
}
//----------------------------------------------------------------------------
void DirectLightingScreenQuad::OnUpdateShaderConstants(int, int)
{
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

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ShadowMapTexture->GetTexture());
    glUniform1i(mShadowMapSamplerLoc, 3);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

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
    glUniform3fv(mLightColorLoc, 1, (GLfloat*)&LightColor);
}
//----------------------------------------------------------------------------
void DirectLightingScreenQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();

    mGBufferPositionSamplerLoc = glGetUniformLocation(program, "GBufferPositionSampler");
    mGBufferNormalSamplerLoc = glGetUniformLocation(program, "GBufferNormalSampler");
    mGBufferAlbedoSamplerLoc = glGetUniformLocation(program, "GBufferAlbedoSampler");
    mShadowMapSamplerLoc = glGetUniformLocation(program, "ShadowMapSampler");
    mLightProjectorViewLoc = glGetUniformLocation(program, "LightProjectorView");
    mLightPositionWorldLoc = glGetUniformLocation(program, "LightPositionWorld");
    mLightColorLoc = glGetUniformLocation(program, "LightColor");
    mLightProjectorNearFarLoc = glGetUniformLocation(program, "LightProjectorNearFar");
}
//----------------------------------------------------------------------------