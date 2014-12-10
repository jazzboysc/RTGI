//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "PassBase.h"

using namespace RTGI;

//----------------------------------------------------------------------------
PassBase::PassBase(const ShaderProgramInfo& programInfo)
{
    mShaderProgram = new ShaderProgram(programInfo);
}
//----------------------------------------------------------------------------
PassBase::PassBase(ShaderProgram* shaderProgram)
{
    mShaderProgram = shaderProgram;
}
//----------------------------------------------------------------------------
PassBase::~PassBase()
{
    mShaderProgram = 0;
    mPSB = 0;
}
//----------------------------------------------------------------------------
ShaderProgram* PassBase::GetShaderProgram()
{
    return mShaderProgram;
}
//----------------------------------------------------------------------------
void PassBase::CreateDeviceResource(GPUDevice* device)
{
    mShaderProgram->CreateDeviceResource(device);
}
//----------------------------------------------------------------------------
void PassBase::SetPipelineStateBlock(PipelineStateBlock* psb)
{
    mPSB = psb;
}
//----------------------------------------------------------------------------
PipelineStateBlock* PassBase::GetPipelineStateBlock() const
{
    return mPSB;
}
//----------------------------------------------------------------------------