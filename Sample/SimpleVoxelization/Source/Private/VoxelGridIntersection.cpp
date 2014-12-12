#include "VoxelGridIntersection.h"
#include "SimpleVoxelizationApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VoxelGridIntersection::VoxelGridIntersection()
{
}
//----------------------------------------------------------------------------
VoxelGridIntersection::~VoxelGridIntersection()
{
}
//----------------------------------------------------------------------------
void VoxelGridIntersection::OnGetShaderConstants()
{
    ComputePass* p = (ComputePass*)GetPass(0);
    ShaderProgram* program = p->GetShaderProgram();

    program->GetUniformLocation(&mSceneBBCenterLoc, "SceneBBCenter");
    program->GetUniformLocation(&mSceneBBExtensionLoc, "SceneBBExtension");
    program->GetUniformLocation(&mDimLoc, "dim");
    program->GetUniformLocation(&mRayStartPointLoc, "RayStartPoint");
    program->GetUniformLocation(&mRayEndPointLoc, "RayEndPoint");
}
//----------------------------------------------------------------------------
void VoxelGridIntersection::OnPreDispatch(unsigned int pass)
{
    vec3 center = SceneBB->GetBoxCenter();
    vec3 extension = SceneBB->GetExtension();
    mSceneBBCenterLoc.SetValue(center);
    mSceneBBExtensionLoc.SetValue(extension);
    mDimLoc.SetValue(SimpleVoxelizationApp::VOXEL_DIMENSION);

    vec3 start, end;
    start.x = SimpleVoxelizationApp::RaySegment[0];
    start.y = SimpleVoxelizationApp::RaySegment[1];
    start.z = SimpleVoxelizationApp::RaySegment[2];
    mRayStartPointLoc.SetValue(start);

    end.x = SimpleVoxelizationApp::RaySegment[3];
    end.y = SimpleVoxelizationApp::RaySegment[4];
    end.z = SimpleVoxelizationApp::RaySegment[5];
    mRayEndPointLoc.SetValue(end);
}
//----------------------------------------------------------------------------
void VoxelGridIntersection::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
//----------------------------------------------------------------------------