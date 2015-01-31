#include "BuildSVOPostAllocateNodes.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BuildSVOPostAllocateNodes::BuildSVOPostAllocateNodes()
{
}
//----------------------------------------------------------------------------
BuildSVOPostAllocateNodes::~BuildSVOPostAllocateNodes()
{
}
//----------------------------------------------------------------------------
void BuildSVOPostAllocateNodes::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void BuildSVOPostAllocateNodes::OnPreDispatch(unsigned int pass)
{
}
//----------------------------------------------------------------------------
void BuildSVOPostAllocateNodes::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
//----------------------------------------------------------------------------