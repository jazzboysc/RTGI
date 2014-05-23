#include "DirectLightingQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
DirectLightingQuad::DirectLightingQuad(Material* material, Light* light)
	:
	ScreenQuad(material),
	mLight(light),
	mLightPositionLoc(0),
	mLightColorLoc(0)
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
void DirectLightingQuad::OnUpdateShaderConstants()
{
	glUniform3fv(mLightPositionLoc, 1, (GLfloat*)&mLight->Location);
	glUniform3fv(mLightColorLoc, 1, (GLfloat*)&mLight->Color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PositionTexture->GetTexture());
	glUniform1i(mPositionSamplerLoc, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NormalTexture->GetTexture());
	glUniform1i(mNormalSamplerLoc, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ReflectanceTexture->GetTexture());
	glUniform1i(mReflectanceSamplerLoc, 2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
//----------------------------------------------------------------------------
void DirectLightingQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mLightPositionLoc = glGetUniformLocation(program, "lightPosition");
	mLightColorLoc = glGetUniformLocation(program, "lightColor");
	mPositionSamplerLoc = glGetUniformLocation(program, "positionSampler");
	mNormalSamplerLoc = glGetUniformLocation(program, "normalSampler");
	mReflectanceSamplerLoc = glGetUniformLocation(program, "reflectanceSampler");
}
//----------------------------------------------------------------------------