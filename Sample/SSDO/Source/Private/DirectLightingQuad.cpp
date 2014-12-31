#include "DirectLightingQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
DirectLightingQuad::DirectLightingQuad(Material* material, Light* light)
	:
	ScreenQuad(material),
	mLight(light)
{
}
//----------------------------------------------------------------------------
DirectLightingQuad::~DirectLightingQuad()
{
	PositionTexture = 0;
	NormalTexture = 0;
	ReflectanceTexture = 0;
}
//----------------------------------------------------------------------------
void DirectLightingQuad::OnUpdateShaderConstants(int, int)
{
	TriangleMesh::OnUpdateShaderConstants(0, 0);

	vec3 lightLoc = mLight->GetLocation();
    mLightPositionLoc.SetValue(lightLoc);
    mLightColorLoc.SetValue(mLight->Color);

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

	mPositionSamplerLoc.SetValue(0);
	mNormalSamplerLoc.SetValue(1);
	mReflectanceSamplerLoc.SetValue(2);
}
//----------------------------------------------------------------------------
void DirectLightingQuad::OnGetShaderConstants()
{
	TriangleMesh::OnGetShaderConstants();

	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mLightPositionLoc, "lightPosition");
    program->GetUniformLocation(&mLightColorLoc, "lightColor");
    program->GetUniformLocation(&mPositionSamplerLoc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerLoc, "normalSampler");
    program->GetUniformLocation(&mReflectanceSamplerLoc, "reflectanceSampler");
}
//----------------------------------------------------------------------------