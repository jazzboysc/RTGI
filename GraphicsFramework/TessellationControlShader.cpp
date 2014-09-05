//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "TessellationControlShader.h"

using namespace RTGI;

//----------------------------------------------------------------------------
TessellationControlShader::TessellationControlShader(
    const std::string& shaderFileName)
	: 
    Shader(GL_TESS_CONTROL_SHADER, shaderFileName)
{
}
//----------------------------------------------------------------------------
TessellationControlShader::~TessellationControlShader()
{
}
//----------------------------------------------------------------------------