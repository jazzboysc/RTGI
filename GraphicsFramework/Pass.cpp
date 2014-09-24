//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Pass.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Pass::Pass(const ShaderProgramInfo& programInfo)
    :
    PassBase(programInfo)
{
}
//----------------------------------------------------------------------------
Pass::Pass(ShaderProgram* shaderProgram)
    :
    PassBase(shaderProgram)
{
}
//----------------------------------------------------------------------------
Pass::~Pass()
{
}
//----------------------------------------------------------------------------
void Pass::Enable()
{
	mShaderProgram->Enable();
}
//----------------------------------------------------------------------------
void Pass::Disable()
{
	mShaderProgram->Disable();
}
//----------------------------------------------------------------------------
bool Pass::IsTessellationEnabled() const
{
    return mShaderProgram->IsTessellationEnabled();
}
//----------------------------------------------------------------------------