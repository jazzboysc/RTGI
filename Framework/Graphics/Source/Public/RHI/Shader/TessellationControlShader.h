//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_TessellationControlShader_H
#define RTGI_TessellationControlShader_H

#include "Shader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/05/2014
//----------------------------------------------------------------------------
class TessellationControlShader : public Shader
{
public:
    TessellationControlShader(const std::string& shaderFileName);
    virtual ~TessellationControlShader();
};

typedef RefPointer<TessellationControlShader> TessellationControlShaderPtr;

}

#endif