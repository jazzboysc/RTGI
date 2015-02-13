//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Shader.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Shader::Shader(GLenum type, const std::string& shaderFileName)
	:
	mType(type),
	mShaderHandle(0)
{
    mShaderFileName = "Shaders/" + shaderFileName;
    bool res = LoadFromFile(mShaderFileName);
	assert( res );

	// TODO:
	// Assign defalut source if LoadFromFile fails.
}
//----------------------------------------------------------------------------
Shader::~Shader()
{
    GPU_DEVICE_FUNC(mShaderHandle->Device, DeleteShader)(this);
    delete mShaderHandle;
}
//----------------------------------------------------------------------------
void Shader::CreateDeviceResource(GPUDevice* device)
{
	if( mShaderHandle )
	{
		return;
	}

    mShaderHandle = GPU_DEVICE_FUNC(device, CreateShader)(this);
}
//----------------------------------------------------------------------------
ShaderHandle* Shader::GetShaderHandle() const
{
	return mShaderHandle;
}
//----------------------------------------------------------------------------
std::string Shader::GetShaderFileName() const
{
    return mShaderFileName;
}
//----------------------------------------------------------------------------
GLenum Shader::GetType() const
{
    return mType;
}
//----------------------------------------------------------------------------
const std::string& Shader::GetSource() const
{
    return mShaderSource;
}
//----------------------------------------------------------------------------
bool Shader::LoadFromFile(const std::string& shaderFileName)
{
	std::ifstream shaderFileData(shaderFileName);
    if( !shaderFileData )
    {
        return false;
    }
	
    size_t startPos, endPos, range;
	std::string curLine;
	while( !shaderFileData.eof() )
	{
		getline(shaderFileData, curLine);

        startPos = curLine.find("#include");
        if( startPos == std::string::npos )
        {
            mShaderSource += curLine + "\n";
        }
        else
        {
            startPos += 8;
            startPos = curLine.find_first_not_of(" \t", startPos);
            startPos += 1;
            endPos = curLine.find_first_of("\"", startPos);
            range = endPos - startPos;
            std::string includeFileName = "Shaders/" + curLine.substr(
                startPos, range);

            bool res = LoadFromFile(includeFileName);
            assert(res);
        }
	}

	return true;
}
//----------------------------------------------------------------------------