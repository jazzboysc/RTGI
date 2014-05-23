//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Pass_H
#define RTGI_Pass_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "ShaderProgram.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 05/18/2014
//----------------------------------------------------------------------------
class Pass : public RefObject
{
public:
	Pass(const std::string& vShaderFileName, const std::string& fShaderFileName);
	Pass(ShaderProgram* shaderProgram);
	~Pass();

	void Enable();
	void Disable();

	ShaderProgram* GetShaderProgram();
	void CreateDeviceResource();

protected:
	ShaderProgramPtr mShaderProgram;
};

typedef RefPointer<Pass> PassPtr;

}

#endif