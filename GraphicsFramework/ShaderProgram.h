//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_ShaderProgram_H
#define RTGI_ShaderProgram_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "VertexShader.h"
#include "FragmentShader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class ShaderProgram : public RefObject
{
public:
	ShaderProgram(const std::string& vShaderFileName, 
		const std::string& fShaderFileName);
	ShaderProgram(VertexShader* vShader, FragmentShader* fShader);
	virtual ~ShaderProgram();

	void CreateDeviceResource();

	GLuint GetProgram() const;
	GLuint GetVertexShader() const;
	GLuint GetFragmentShader() const;

	void Enable();
	void Disable();

protected:
	// TODO:
	// Get vertex attribute locations, uniform locations here?
	//virtual void OnInitDeviceResource() = 0;

	VertexShaderPtr mVertexShader;
	FragmentShaderPtr mFragmentShader;
	GLuint mProgram;
};

typedef RefPointer<ShaderProgram> ShaderProgramPtr;

}

#endif