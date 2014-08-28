//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "ComputeShader.h"

#ifndef __APPLE__

using namespace RTGI;

//----------------------------------------------------------------------------
ComputeShader::ComputeShader(const std::string& shaderFileName)
	: 
	Shader(GL_COMPUTE_SHADER, shaderFileName)
{
}
//----------------------------------------------------------------------------
ComputeShader::~ComputeShader()
{
}
//----------------------------------------------------------------------------

#endif