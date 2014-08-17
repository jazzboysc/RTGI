#include "UpdateAccumulationScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
UpdateAccumulationScreenQuad::UpdateAccumulationScreenQuad(Material* material)
	:
	ScreenQuad(material),
	WorldRayBundleDirection(0.0f),
	mWorldRayBundleDirectionLoc(0),
	mVoxelGridCenterLoc(0)
{
	VoxelGridCenter = vec3(0.0f);
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
	glUniform3fv(mVoxelGridCenterLoc, 1, (GLfloat*)&VoxelGridCenter);
}
//----------------------------------------------------------------------------
void UpdateAccumulationScreenQuad::OnGetShaderConstants()
{
	GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();
	mWorldRayBundleDirectionLoc = glGetUniformLocation(program, 
		"worldRayBundleDirection");
	mVoxelGridCenterLoc = glGetUniformLocation(program, 
		"voxelGridCenter");
}
//----------------------------------------------------------------------------