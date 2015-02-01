#include "BuildSVO.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BuildSVO::BuildSVO()
{
}
//----------------------------------------------------------------------------
BuildSVO::~BuildSVO()
{
}
//----------------------------------------------------------------------------
void BuildSVO::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void BuildSVO::OnPreDispatch(unsigned int pass)
{
}
//----------------------------------------------------------------------------
void BuildSVO::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT | 
        GL_ATOMIC_COUNTER_BARRIER_BIT);
}
//----------------------------------------------------------------------------