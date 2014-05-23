//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Shader.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Shader::Shader(GLenum type, const std::string& shaderFileName)
	:
	mType(type),
	mShader(0)
{
	bool res = LoadFromFile(shaderFileName);
	assert( res );

	// TODO:
	// Assign defalut source if LoadFromFile fails.
}
//----------------------------------------------------------------------------
Shader::~Shader()
{
	glDeleteShader(mShader);
	mShader = 0;
}
//----------------------------------------------------------------------------
void Shader::CreateDeviceResource()
{
	if( mShader )
	{
		return;
	}

	const char* programText = mShaderSource.c_str();

	mShader = glCreateShader(mType);
	glShaderSource(mShader, 1, &programText, 0);
	glCompileShader(mShader);

	GLint compiled;
	glGetShaderiv(mShader, GL_COMPILE_STATUS, &compiled);
	if( !compiled )
	{
        GLint iInfoLen = 0;
        glGetShaderiv(mShader, GL_INFO_LOG_LENGTH, &iInfoLen);
        if( iInfoLen > 1 )
        {
            char* acInfoLog = new char[iInfoLen];
            glGetShaderInfoLog(mShader, iInfoLen, 0, acInfoLog); 

            delete[] acInfoLog;
        }

		assert( false );
	}
}
//----------------------------------------------------------------------------
GLuint Shader::GetShader() const
{
	return mShader;
}
//----------------------------------------------------------------------------
bool Shader::LoadFromFile(const std::string& shaderFileName)
{
	std::ifstream shaderFileData(shaderFileName);
    if( !shaderFileData )
    {
        return false;
    }
	
	std::string curLine;
	while( !shaderFileData.eof() )
	{
		getline(shaderFileData, curLine);
		mShaderSource += curLine + "\n";
	}

	return true;
}
//----------------------------------------------------------------------------