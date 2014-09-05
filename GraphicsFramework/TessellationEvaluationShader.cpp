//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "TessellationEvaluationShader.h"

using namespace RTGI;

//----------------------------------------------------------------------------
TessellationEvaluationShader::TessellationEvaluationShader(
    const std::string& shaderFileName)
	: 
    Shader(GL_TESS_EVALUATION_SHADER, shaderFileName)
{
}
//----------------------------------------------------------------------------
TessellationEvaluationShader::~TessellationEvaluationShader()
{
}
//----------------------------------------------------------------------------