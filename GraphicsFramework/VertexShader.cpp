//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "VertexShader.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VertexShader::VertexShader(const std::string& shaderFileName)
	: 
	Shader(GL_VERTEX_SHADER, shaderFileName)
{
}
//----------------------------------------------------------------------------
VertexShader::~VertexShader()
{
}
//----------------------------------------------------------------------------