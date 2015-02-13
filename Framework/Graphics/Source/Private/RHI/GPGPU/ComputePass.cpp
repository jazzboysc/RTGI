//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "ComputePass.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ComputePass::ComputePass(const ShaderProgramInfo& programInfo)
    :
    PassBase(programInfo)
{
    if( (programInfo.ShaderStageFlag & ShaderType::ST_Compute) != 0 )
    {
        mIsVertexPass = false;
    }
    else if( (programInfo.ShaderStageFlag & ShaderType::ST_Vertex) != 0 )
    {
        mIsVertexPass = true;
    }
    else
    {
        assert(false);
    }
}
//----------------------------------------------------------------------------
ComputePass::ComputePass(ShaderProgram* computeProgram)
    :
    PassBase(computeProgram)
{
    if( computeProgram->IsComputeEnabled() )
    {
        mIsVertexPass = false;
    }
    else if( computeProgram->IsVertexEnabled() )
    {
        mIsVertexPass = true;
    }
    else
    {
        assert(false);
    }
}
//----------------------------------------------------------------------------
ComputePass::ComputePass()
    :
    PassBase(0)
{
}
//----------------------------------------------------------------------------
ComputePass::~ComputePass()
{
}
//----------------------------------------------------------------------------