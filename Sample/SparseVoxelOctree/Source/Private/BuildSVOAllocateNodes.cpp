#include "BuildSVOAllocateNodes.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BuildSVOAllocateNodes::BuildSVOAllocateNodes()
{
}
//----------------------------------------------------------------------------
BuildSVOAllocateNodes::~BuildSVOAllocateNodes()
{
}
//----------------------------------------------------------------------------
void BuildSVOAllocateNodes::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void BuildSVOAllocateNodes::OnPreDispatch(unsigned int pass)
{
}
//----------------------------------------------------------------------------
void BuildSVOAllocateNodes::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
//----------------------------------------------------------------------------