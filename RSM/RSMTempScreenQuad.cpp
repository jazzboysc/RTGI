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
void RSMTempScreenQuad::OnUpdateShaderConstants(int, int)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, TempTexture->GetTexture());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    
    float nearFarPlane[2];
    mCamera->GetNearFarPlane(nearFarPlane);
    mNearFarPlaneLoc.SetValue(nearFarPlane);
    mShowModeLoc.SetValue(ShowMode);
    mTempSamplerLoc.SetValue(0);
}
//----------------------------------------------------------------------------
void RSMTempScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
    program->GetUniformLocation(&mNearFarPlaneLoc, "nearFarPlane");
    program->GetUniformLocation(&mShowModeLoc, "showMode");
}
//----------------------------------------------------------------------------