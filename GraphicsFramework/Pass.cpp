//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Pass.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Pass::Pass(const ShaderProgramInfo& programInfo)
{
    mShaderProgram = new ShaderProgram(programInfo);
}
//----------------------------------------------------------------------------
Pass::Pass(ShaderProgram* shaderProgram)
{
	mShaderProgram = shaderProgram;
}
//----------------------------------------------------------------------------
Pass::~Pass()
{
	mShaderProgram = 0;
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
ShaderProgram* Pass::GetShaderProgram()
{
	return mShaderProgram;
}
//----------------------------------------------------------------------------
void Pass::CreateDeviceResource()
{
	mShaderProgram->CreateDeviceResource();
}
//----------------------------------------------------------------------------