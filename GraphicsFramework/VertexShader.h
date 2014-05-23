//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_VertexShader_H
#define RTGI_VertexShader_H

#include "Shader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class VertexShader : public Shader
{
public:
	VertexShader(const std::string& shaderFileName);
	virtual ~VertexShader();
};

typedef RefPointer<VertexShader> VertexShaderPtr;

}

#endif