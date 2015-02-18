#include "SVORayIntersection.h"
#include "SVOApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SVORayIntersection::SVORayIntersection()
{
}
//----------------------------------------------------------------------------
SVORayIntersection::~SVORayIntersection()
{
}
//----------------------------------------------------------------------------
void SVORayIntersection::OnGetShaderConstants()
{
    ComputePass* p = (ComputePass*)GetPass(0);
    ShaderProgram* program = p->GetShaderProgram();

    program->GetUniformLocation(&mRayStartPointLoc, "RayStartPoint");
    program->GetUniformLocation(&mRayEndPointLoc, "RayEndPoint");
}
//----------------------------------------------------------------------------
void SVORayIntersection::OnPreDispatch(unsigned int pass)
{
    vec3 start, end;
    start.x = SVOApp::RaySegment[0];
    start.y = SVOApp::RaySegment[1];
    start.z = SVOApp::RaySegment[2];
    mRayStartPointLoc.SetValue(start);

    end.x = SVOApp::RaySegment[3];
    end.y = SVOApp::RaySegment[4];
    end.z = SVOApp::RaySegment[5];
    mRayEndPointLoc.SetValue(end);
}
//----------------------------------------------------------------------------
void SVORayIntersection::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
//----------------------------------------------------------------------------