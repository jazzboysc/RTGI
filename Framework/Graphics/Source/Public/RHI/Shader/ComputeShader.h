//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ComputeShader_H
#define RTGI_ComputeShader_H

#ifndef __APPLE__

#include "Shader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/16/2014
//----------------------------------------------------------------------------
class ComputeShader : public Shader
{
public:
	ComputeShader(const std::string& shaderFileName);
	virtual ~ComputeShader();
};

typedef RefPointer<ComputeShader> ComputeShaderPtr;

}

#endif

#endif