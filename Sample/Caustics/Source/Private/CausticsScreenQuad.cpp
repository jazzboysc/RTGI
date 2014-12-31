#include "CausticsScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
CausticsScreenQuad::CausticsScreenQuad(Material* material, Camera* camera)
	:
	ScreenQuad(material, camera)
{
}
//----------------------------------------------------------------------------
CausticsScreenQuad::~CausticsScreenQuad()
{
	PositionTexture = 0;
	NormalTexture = 0;
	ReflectanceTexture = 0;
}
//----------------------------------------------------------------------------
void CausticsScreenQuad::OnUpdateShaderConstants(int technique, int pass)
{
	TriangleMesh::OnUpdateShaderConstants(technique, pass);

	vec3 lightLoc = Light->GetLocation();
	mLightPositionLoc.SetValue(lightLoc);
	mLightColorLoc.SetValue(Light->Color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, PositionTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, NormalTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, ReflectanceTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    mPositionSamplerLoc.SetValue(0);
    mNormalSamplerLoc.SetValue(1);
    mReflectanceSamplerLoc.SetValue(2);
}
//----------------------------------------------------------------------------
void CausticsScreenQuad::OnGetShaderConstants()
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