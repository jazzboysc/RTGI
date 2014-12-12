#include "VPLDirectLightingScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLDirectLightingScreenQuad::VPLDirectLightingScreenQuad(Material* material)
	:
	ScreenQuad(material, 0)
{
    LightColor = vec3(0.9686f, 0.9333f, 0.8392f)*0.8f;
}
//----------------------------------------------------------------------------
VPLDirectLightingScreenQuad::~VPLDirectLightingScreenQuad()
{
    GBufferPositionTexture = 0;
    GBufferNormalTexture = 0;
    GBufferAlbedoTexture = 0;
    ShadowMapTexture = 0;
}
//----------------------------------------------------------------------------
void VPLDirectLightingScreenQuad::OnUpdateShaderConstants(int, int)
{
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

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, ShadowMapTexture->GetTexture());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    mGBufferPositionSamplerLoc.SetValue(0);
    mGBufferNormalSamplerLoc.SetValue(1);
    mGBufferAlbedoSamplerLoc.SetValue(2);
    mShadowMapSamplerLoc.SetValue(3);

    assert( LightProjector );
    if( LightProjector )
    {
        mat4 viewTrans = LightProjector->GetViewTransform();
        mLightProjectorViewLoc.SetValue(viewTrans);

        vec3 lightPosition = LightProjector->GetLocation();
        mLightPositionWorldLoc.SetValue(lightPosition);

        float nearFarPlane[2];
        LightProjector->GetNearFarPlane(nearFarPlane);
        mLightProjectorNearFarLoc.SetValue(nearFarPlane);
    }
    mLightColorLoc.SetValue(LightColor);
}
//----------------------------------------------------------------------------
void VPLDirectLightingScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);

    program->GetUniformLocation(&mGBufferPositionSamplerLoc, "GBufferPositionSampler");
    program->GetUniformLocation(&mGBufferNormalSamplerLoc, "GBufferNormalSampler");
    program->GetUniformLocation(&mGBufferAlbedoSamplerLoc, "GBufferAlbedoSampler");
    program->GetUniformLocation(&mShadowMapSamplerLoc, "ShadowMapSampler");
    program->GetUniformLocation(&mLightProjectorViewLoc, "LightProjectorView");
    program->GetUniformLocation(&mLightPositionWorldLoc, "LightPositionWorld");
    program->GetUniformLocation(&mLightColorLoc, "LightColor");
    program->GetUniformLocation(&mLightProjectorNearFarLoc, "LightProjectorNearFar");
}
//----------------------------------------------------------------------------