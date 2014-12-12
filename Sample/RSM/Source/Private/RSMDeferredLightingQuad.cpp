#include "RSMDeferredLightingQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMDeferredLightingQuad::RSMDeferredLightingQuad(Material* material,
    Light* light)
	:
	ScreenQuad(material),
	mLight(light)
{
}
//----------------------------------------------------------------------------
RSMDeferredLightingQuad::~RSMDeferredLightingQuad()
{
	PositionTexture = 0;
	NormalTexture = 0;
	ReflectanceTexture = 0;
    IndirectLightingTexture = 0;
}
//----------------------------------------------------------------------------
void RSMDeferredLightingQuad::OnUpdateShaderConstants(int, int)
{
    vec3 lightLoc = mLight->GetLocation();
    mLightPositionLoc.SetValue(lightLoc);
    mLightColorLoc.SetValue(mLight->Color);
    mPositionSamplerLoc.SetValue(0);
    mNormalSamplerLoc.SetValue(1);
    mReflectanceSamplerLoc.SetValue(2);
    mIndirectLightingSamplerLoc.SetValue(3);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PositionTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NormalTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ReflectanceTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, IndirectLightingTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
//----------------------------------------------------------------------------
void RSMDeferredLightingQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mLightPositionLoc, "lightPosition");
    program->GetUniformLocation(&mLightColorLoc, "lightColor");
    program->GetUniformLocation(&mPositionSamplerLoc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerLoc, "normalSampler");
    program->GetUniformLocation(&mReflectanceSamplerLoc, "reflectanceSampler");
    program->GetUniformLocation(&mIndirectLightingSamplerLoc, 
        "indirectLightingSampler");
}
//----------------------------------------------------------------------------