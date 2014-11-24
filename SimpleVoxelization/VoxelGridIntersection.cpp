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
    GLuint program = p->GetShaderProgram()->GetProgram();

    mSceneBBCenterLoc = glGetUniformLocation(program, "SceneBBCenter");
    mSceneBBExtensionLoc = glGetUniformLocation(program, "SceneBBExtension");
    mDimLoc = glGetUniformLocation(program, "Dim");
    mRayStartPointLoc = glGetUniformLocation(program, "RayStartPoint");
    mRayEndPointLoc = glGetUniformLocation(program, "RayEndPoint");
}
//----------------------------------------------------------------------------
void VoxelGridIntersection::OnPreDispatch(unsigned int pass)
{
    vec3 center = SceneBB->GetBoxCenter();
    vec3 extension = SceneBB->GetExtension();
    glUniform3fv(mSceneBBCenterLoc, 1, (GLfloat*)&center);
    glUniform3fv(mSceneBBExtensionLoc, 1, (GLfloat*)&extension);
    glUniform1i(mDimLoc, SimpleVoxelizationApp::VOXEL_DIMENSION);
    glUniform3fv(mRayStartPointLoc, 1, (GLfloat*)&SimpleVoxelizationApp::RaySegment[0]);
    glUniform3fv(mRayEndPointLoc, 1, (GLfloat*)&SimpleVoxelizationApp::RaySegment[3]);
}
//----------------------------------------------------------------------------
void VoxelGridIntersection::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
//----------------------------------------------------------------------------