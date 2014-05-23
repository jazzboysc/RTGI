//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "FragmentShader.h"

using namespace RTGI;

//----------------------------------------------------------------------------
FragmentShader::FragmentShader(const std::string& shaderFileName)
	: 
	Shader(GL_FRAGMENT_SHADER, shaderFileName)
{
}
//----------------------------------------------------------------------------
FragmentShader::~FragmentShader()
{
}
//----------------------------------------------------------------------------