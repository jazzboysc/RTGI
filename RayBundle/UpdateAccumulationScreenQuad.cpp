#include "UpdateAccumulationScreenQuad.h"

using namespace RTGI;

//----------------------------------------------------------------------------
UpdateAccumulationScreenQuad::UpdateAccumulationScreenQuad(Material* material)
	:
	ScreenQuad(material),
	WorldRayBundleDirection(0.0f)
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
    mWorldRayBundleDirectionLoc.SetValue(WorldRayBundleDirection);
    mVoxelGridCenterLoc.SetValue(VoxelGridCenter);
}
//----------------------------------------------------------------------------
void UpdateAccumulationScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mWorldRayBundleDirectionLoc, 
        "worldRayBundleDirection");
    program->GetUniformLocation(&mVoxelGridCenterLoc, 
        "voxelGridCenter");
}
//----------------------------------------------------------------------------