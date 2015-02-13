//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "GeometryShader.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GeometryShader::GeometryShader(const std::string& shaderFileName)
	: 
	Shader(GL_GEOMETRY_SHADER, shaderFileName)
{
}
//----------------------------------------------------------------------------
GeometryShader::~GeometryShader()
{
}
//----------------------------------------------------------------------------