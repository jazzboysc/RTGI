//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_TessellationEvaluationShader_H
#define RTGI_TessellationEvaluationShader_H

#include "Shader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/05/2014
//----------------------------------------------------------------------------
class TessellationEvaluationShader : public Shader
{
public:
    TessellationEvaluationShader(const std::string& shaderFileName);
    virtual ~TessellationEvaluationShader();
};

typedef RefPointer<TessellationEvaluationShader> TessellationEvaluationShaderPtr;

}

#endif