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
    switch( pass )
    {
    case BUILD_SVO_INIT_ROOT_PASS:
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        break;

    case BUILD_SVO_FLAG_NODES_PASS:
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        break;

    case BUILD_SVO_ALLOC_NODES_PASS:
        glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
        break;

    case BUILD_SVO_POST_ALLOC_NODES_PASS:
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);
        break;

    case BUILD_SVO_INIT_NODES_PASS:
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        break;

    default:
        break;
    }
}
//----------------------------------------------------------------------------