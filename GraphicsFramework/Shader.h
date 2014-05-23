//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Shader_H
#define RTGI_Shader_H

#include "FrameworkCommon.h"
#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class Shader : public RefObject
{
public:
	Shader(GLenum type, const std::string& shaderFileName);
	virtual ~Shader();

	void CreateDeviceResource();
	GLuint GetShader() const;

protected:
	bool LoadFromFile(const std::string& shaderFileName);

	std::string mShaderSource;
	GLenum mType;
	GLuint mShader;
};

typedef RefPointer<Shader> ShaderPtr;

}

#endif
