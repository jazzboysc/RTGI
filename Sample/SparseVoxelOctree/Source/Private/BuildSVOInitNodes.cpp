#include "BuildSVOInitNodes.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BuildSVOInitNodes::BuildSVOInitNodes()
{
}
//----------------------------------------------------------------------------
BuildSVOInitNodes::~BuildSVOInitNodes()
{
}
//----------------------------------------------------------------------------
void BuildSVOInitNodes::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void BuildSVOInitNodes::OnPreDispatch(unsigned int pass)
{
}
//----------------------------------------------------------------------------
void BuildSVOInitNodes::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
//----------------------------------------------------------------------------