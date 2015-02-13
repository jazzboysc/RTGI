//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_GeometryShader_H
#define RTGI_GeometryShader_H

#include "Shader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/05/2014
//----------------------------------------------------------------------------
class GeometryShader : public Shader
{
public:
    GeometryShader(const std::string& shaderFileName);
    virtual ~GeometryShader();
};

typedef RefPointer<GeometryShader> GeometryShaderPtr;

}

#endif