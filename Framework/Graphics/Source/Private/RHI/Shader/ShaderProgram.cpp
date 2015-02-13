//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "ShaderProgram.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ShaderProgram::ShaderProgram(const ShaderProgramInfo& programInfo)
	:
	mProgramHandle(0)
{
    if( programInfo.ShaderStageFlag & ShaderType::ST_Vertex )
    {
        mVertexShader = new VertexShader(programInfo.VShaderFileName);
    }

    if( programInfo.ShaderStageFlag & ShaderType::ST_Fragment )
    {
        mFragmentShader = new FragmentShader(programInfo.FShaderFileName);
    }

    if( programInfo.ShaderStageFlag & ShaderType::ST_Geometry )
    {
        mGeometryShader = new GeometryShader(programInfo.GShaderFileName);
    }

    if( programInfo.ShaderStageFlag & ShaderType::ST_Compute )
    {
        mComputeShader = new ComputeShader(programInfo.CShaderFileName);
    }

    if( programInfo.ShaderStageFlag & ShaderType::ST_TessellationControl )
    {
        mTessellationControlShader = new TessellationControlShader(
            programInfo.TCShaderFileName);
    }

    if( programInfo.ShaderStageFlag & ShaderType::ST_TessellationEvaluation )
    {
        mTessellationEvaluationShader = new TessellationEvaluationShader(
            programInfo.TEShaderFileName);
    }
    mProgramInfo = programInfo;
}
//----------------------------------------------------------------------------
ShaderProgram::ShaderProgram(VertexShader* vShader, FragmentShader* fShader)
	:
	mProgramHandle(0)
{
	assert( vShader && fShader );
	mVertexShader = vShader;
	mFragmentShader = fShader;
}
//----------------------------------------------------------------------------
ShaderProgram::~ShaderProgram()
{
	mVertexShader = 0;
	mFragmentShader = 0;
    mGeometryShader = 0;
    mComputeShader = 0;
    mTessellationControlShader = 0;
    mTessellationEvaluationShader = 0;

    GPU_DEVICE_FUNC(mProgramHandle->Device, DeleteProgram)(this);
    delete mProgramHandle;
}
//----------------------------------------------------------------------------
void ShaderProgram::CreateDeviceResource(GPUDevice* device)
{
	if( mProgramHandle )
	{
		return;
	}

    mProgramHandle = GPU_DEVICE_FUNC(device, CreateProgram)(this);
}
//----------------------------------------------------------------------------
void ShaderProgram::GetUniformLocation(ShaderUniform* dstUniform, 
    const char* name)
{
    GPU_DEVICE_FUNC_GetUniformLocation(this, *dstUniform, name);
}
//----------------------------------------------------------------------------
ShaderProgramHandle* ShaderProgram::GetProgramHandle() const
{
	return mProgramHandle;
}
//----------------------------------------------------------------------------
VertexShader* ShaderProgram::GetVertexShader() const
{
	return mVertexShader;
}
//----------------------------------------------------------------------------
FragmentShader* ShaderProgram::GetFragmentShader() const
{
	return mFragmentShader;
}
//----------------------------------------------------------------------------
GeometryShader* ShaderProgram::GetGeometryShader() const
{
    return mGeometryShader;
}
//----------------------------------------------------------------------------
ComputeShader* ShaderProgram::GetComputeShader() const
{
    return mComputeShader;
}
//----------------------------------------------------------------------------
TessellationControlShader* ShaderProgram::GetTessellationControlShader() const
{
    return mTessellationControlShader;
}
//----------------------------------------------------------------------------
TessellationEvaluationShader* ShaderProgram::GetTessellationEvaluationShader(
    ) const
{
    return mTessellationEvaluationShader;
}
//----------------------------------------------------------------------------
void ShaderProgram::Enable()
{
    GPU_DEVICE_FUNC(mProgramHandle->Device, EnableProgram)(this);
}
//----------------------------------------------------------------------------
void ShaderProgram::Disable()
{
    GPU_DEVICE_FUNC(mProgramHandle->Device, DisableProgram)(this);
}
//----------------------------------------------------------------------------
bool ShaderProgram::IsTessellationEnabled() const
{
    if( mProgramInfo.ShaderStageFlag & ShaderType::ST_TessellationEvaluation )
    {
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool ShaderProgram::IsComputeEnabled() const
{
    if( mProgramInfo.ShaderStageFlag & ShaderType::ST_Compute )
    {
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
bool ShaderProgram::IsVertexEnabled() const
{
    if( mProgramInfo.ShaderStageFlag & ShaderType::ST_Vertex )
    {
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------