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
        GLint iInfoLen = 0;
        glGetProgramiv(mProgram, GL_INFO_LOG_LENGTH, &iInfoLen);
        if( iInfoLen > 1 )
        {
            char* acInfoLog = new char[iInfoLen];
            glGetProgramInfoLog(mProgram, iInfoLen, 0, acInfoLog);
            printf("Failed linking %s, %s\n%s\n",
                   mVertexShader->GetShaderFileName().c_str(),
                   mFragmentShader->GetShaderFileName().c_str(),
                   acInfoLog);
            
            delete[] acInfoLog;
        }
        
		assert( false );
	}

#ifdef RTGI_OUTPUT_RESOURCE_LOADING
    printf("Linking program %s, %s finished\n",
           mVertexShader->GetShaderFileName().c_str(),
           mFragmentShader->GetShaderFileName().c_str());
#endif
    
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