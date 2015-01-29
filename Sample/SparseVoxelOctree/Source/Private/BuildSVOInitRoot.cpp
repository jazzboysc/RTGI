#include "BuildSVOInitRoot.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BuildSVOInitRoot::BuildSVOInitRoot()
{
}
//----------------------------------------------------------------------------
BuildSVOInitRoot::~BuildSVOInitRoot()
{
}
//----------------------------------------------------------------------------
void BuildSVOInitRoot::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void BuildSVOInitRoot::OnPreDispatch(unsigned int pass)
{
}
//----------------------------------------------------------------------------
void BuildSVOInitRoot::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
//----------------------------------------------------------------------------