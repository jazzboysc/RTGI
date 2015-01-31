#include "BuildSVOFlagNodes.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BuildSVOFlagNodes::BuildSVOFlagNodes()
{
}
//----------------------------------------------------------------------------
BuildSVOFlagNodes::~BuildSVOFlagNodes()
{
}
//----------------------------------------------------------------------------
void BuildSVOFlagNodes::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void BuildSVOFlagNodes::OnPreDispatch(unsigned int pass)
{
}
//----------------------------------------------------------------------------
void BuildSVOFlagNodes::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
//----------------------------------------------------------------------------