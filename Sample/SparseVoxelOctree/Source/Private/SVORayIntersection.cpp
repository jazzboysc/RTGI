#include "SVORayIntersection.h"

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
}
//----------------------------------------------------------------------------
void SVORayIntersection::OnPreDispatch(unsigned int pass)
{

}
//----------------------------------------------------------------------------
void SVORayIntersection::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
//----------------------------------------------------------------------------