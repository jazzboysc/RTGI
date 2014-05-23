//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_FragmentShader_H
#define RTGI_FragmentShader_H

#include "Shader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class FragmentShader : public Shader
{
public:
	FragmentShader(const std::string& shaderFileName);
	virtual ~FragmentShader();
};

typedef RefPointer<FragmentShader> FragmentShaderPtr;

}

#endif