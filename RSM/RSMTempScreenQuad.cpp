#include "RSMTempScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMTempScreenQuad::RSMTempScreenQuad(Material* material, Camera* camera)
	:
	ScreenQuad(material, camera)
{
    ShowMode = 2;
}
//----------------------------------------------------------------------------
RSMTempScreenQuad::~RSMTempScreenQuad()
{
	TempTexture = 0;
}
//----------------------------------------------------------------------------
void RSMTempScreenQuad::OnUpdateShaderConstants()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TempTexture->GetTexture());
	glUniform1i(mTempSamplerLoc, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    float nearFarPlane[2];
    mCamera->GetNearFarPlane(nearFarPlane);
    glUniform2fv(mNearFarPlaneLoc, 1, nearFarPlane);
    glUniform1i(mShowModeLoc, ShowMode);
}
//----------------------------------------------------------------------------
void RSMTempScreenQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mTempSamplerLoc = glGetUniformLocation(program, "tempSampler");
    mNearFarPlaneLoc = glGetUniformLocation(program, "nearFarPlane");
    mShowModeLoc = glGetUniformLocation(program, "showMode");
}
//----------------------------------------------------------------------------