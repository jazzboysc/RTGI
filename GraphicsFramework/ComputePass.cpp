//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "ComputePass.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ComputePass::ComputePass(const ShaderProgramInfo& programInfo)
    :
    PassBase(programInfo)
{
    assert( (programInfo.ShaderStageFlag & ShaderType::ST_Compute) != 0 );
}
//----------------------------------------------------------------------------
ComputePass::ComputePass(ShaderProgram* computeProgram)
    :
    PassBase(computeProgram)
{
    assert( computeProgram->IsComputeEnabled() );
}
//----------------------------------------------------------------------------
ComputePass::~ComputePass()
{
}
//----------------------------------------------------------------------------