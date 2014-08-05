#include "UpdateAccumulationScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
UpdateAccumulationScreenQuad::UpdateAccumulationScreenQuad(Material* material)
	:
	ScreenQuad(material),
	WorldRayBundleDirection(0.0f),
	mWorldRayBundleDirectionLoc(0)
{
}
//----------------------------------------------------------------------------
UpdateAccumulationScreenQuad::~UpdateAccumulationScreenQuad()
{
}
//----------------------------------------------------------------------------
void UpdateAccumulationScreenQuad::OnUpdateShaderConstants(int, int)
{
	glUniform3fv(mWorldRayBundleDirectionLoc, 1, 
		(GLfloat*)&WorldRayBundleDirection);
}
//----------------------------------------------------------------------------
void UpdateAccumulationScreenQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mWorldRayBundleDirectionLoc = glGetUniformLocation(program, 
		"worldRayBundleDirection");
}
//----------------------------------------------------------------------------