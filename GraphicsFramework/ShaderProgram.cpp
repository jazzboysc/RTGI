//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "ShaderProgram.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ShaderProgram::ShaderProgram(const ShaderProgramInfo& programInfo)
	:
	mProgram(0)
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
	mProgram(0)
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
	glDeleteProgram(mProgram);
	mProgram = 0;
}
//----------------------------------------------------------------------------
void ShaderProgram::CreateDeviceResource()
{
	if( mProgram )
	{
		return;
	}

    mProgram = glCreateProgram();
    std::string linkingString("Linking program");

    if( mVertexShader )
    {
        mVertexShader->CreateDeviceResource();
        glAttachShader(mProgram, mVertexShader->GetShader());
        linkingString += " ";
        linkingString +=  mVertexShader->GetShaderFileName();
    }
    if( mFragmentShader )
    {
        mFragmentShader->CreateDeviceResource();
        glAttachShader(mProgram, mFragmentShader->GetShader());
        linkingString += " ";
        linkingString += mFragmentShader->GetShaderFileName();
    }
    if( mGeometryShader )
    {
        mGeometryShader->CreateDeviceResource();
        glAttachShader(mProgram, mGeometryShader->GetShader());
        linkingString += " ";
        linkingString += mGeometryShader->GetShaderFileName();
    }
    if( mComputeShader )
    {
        mComputeShader->CreateDeviceResource();
        glAttachShader(mProgram, mComputeShader->GetShader());
        linkingString += " ";
        linkingString += mComputeShader->GetShaderFileName();
    }
    if( mTessellationControlShader )
    {
        mTessellationControlShader->CreateDeviceResource();
        glAttachShader(mProgram, mTessellationControlShader->GetShader());
        linkingString += " ";
        linkingString += mTessellationControlShader->GetShaderFileName();
    }
    if( mTessellationEvaluationShader )
    {
        mTessellationEvaluationShader->CreateDeviceResource();
        glAttachShader(mProgram, mTessellationEvaluationShader->GetShader());
        linkingString += " ";
        linkingString += mTessellationEvaluationShader->GetShaderFileName();
    }

    glLinkProgram(mProgram);
    GLint linked;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &linked);
    if( !linked )
	{
        GLint iInfoLen = 0;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &iInfoLen);
        if( iInfoLen > 1 )
        {
            char* acInfoLog = new char[iInfoLen];
            glGetProgramInfoLog(mProgram, iInfoLen, 0, acInfoLog);
            linkingString += " failed";

            printf("%s\n%s\n", linkingString.c_str(), acInfoLog);
            delete[] acInfoLog;
        }
        
		assert( false );
	}

#ifdef RTGI_OUTPUT_RESOURCE_LOADING
    linkingString += " finished";
    printf("%s\n", linkingString.c_str());
#endif
}
//----------------------------------------------------------------------------
GLuint ShaderProgram::GetProgram() const
{
	return mProgram;
}
//----------------------------------------------------------------------------
GLuint ShaderProgram::GetVertexShader() const
{
    assert( mVertexShader );
	return mVertexShader->GetShader();
}
//----------------------------------------------------------------------------
GLuint ShaderProgram::GetFragmentShader() const
{
    assert( mFragmentShader );
	return mFragmentShader->GetShader();
}
//----------------------------------------------------------------------------
GLuint ShaderProgram::GetGeometryShader() const
{
    assert(mGeometryShader);
    return mGeometryShader->GetShader();
}
//----------------------------------------------------------------------------
GLuint ShaderProgram::GetComputeShader() const
{
    assert(mComputeShader);
    return mComputeShader->GetShader();
}
//----------------------------------------------------------------------------
GLuint ShaderProgram::GetTessellationControlShader() const
{
    assert(mTessellationControlShader);
    return mTessellationControlShader->GetShader();
}
//----------------------------------------------------------------------------
GLuint ShaderProgram::GetTessellationEvaluationShader() const
{
    assert(mTessellationEvaluationShader);
    return mTessellationEvaluationShader->GetShader();
}
//----------------------------------------------------------------------------
void ShaderProgram::Enable()
{
	glUseProgram(mProgram);
}
//----------------------------------------------------------------------------
void ShaderProgram::Disable()
{
	// Nothing to do.
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