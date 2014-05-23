//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "ShaderProgram.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ShaderProgram::ShaderProgram(const std::string& vShaderFileName, 
	const std::string& fShaderFileName)
	:
	mProgram(0)
{
	mVertexShader = new VertexShader(vShaderFileName);
	mFragmentShader = new FragmentShader(fShaderFileName);
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

	mVertexShader->CreateDeviceResource();
	mFragmentShader->CreateDeviceResource();

	mProgram = glCreateProgram();
	glAttachShader(mProgram, mVertexShader->GetShader());
	glAttachShader(mProgram, mFragmentShader->GetShader());

    glLinkProgram(mProgram);
    GLint linked;
    glGetProgramiv(mProgram, GL_LINK_STATUS, &linked);
    if( !linked )
	{
		assert( false );
		// TODO:
		// Output error info.
	}

	//OnInitDeviceResource();
}
//----------------------------------------------------------------------------
GLuint ShaderProgram::GetProgram() const
{
	return mProgram;
}
//----------------------------------------------------------------------------
GLuint ShaderProgram::GetVertexShader() const
{
	return mVertexShader->GetShader();
}
//----------------------------------------------------------------------------
GLuint ShaderProgram::GetFragmentShader() const
{
	return mFragmentShader->GetShader();
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