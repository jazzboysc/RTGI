//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Shader_H
#define RTGI_Shader_H

#include "RefObject.h"
#include "GPUDevice.h"

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

	void CreateDeviceResource(GPUDevice* device);
    ShaderHandle* GetShaderHandle() const;
    std::string GetShaderFileName() const;
    GLenum  GetType() const;
    const std::string&  GetSource() const;

protected:
	bool LoadFromFile(const std::string& shaderFileName);

    std::string mShaderFileName;
	std::string mShaderSource;
	GLenum mType;
    ShaderHandle*  mShaderHandle;
};

typedef RefPointer<Shader> ShaderPtr;

}

#endif
