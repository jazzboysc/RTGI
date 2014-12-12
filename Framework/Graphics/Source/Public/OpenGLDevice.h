//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_OpenGLDevice_H
#define RTGI_OpenGLDevice_H

#ifdef __APPLE__  // include Mac OS X verions of headers
#  include <OpenGL/OpenGL.h>
#  include <GLUT/glut.h>
#else // non-Mac OS X operating systems
#  include <GL/glew.h>
#  include <freeglut.h>
#  include <freeglut_ext.h>
#endif  // __APPLE__

#include "GPUDevice.h"
#include "OpenGLResource.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/08/2014
//----------------------------------------------------------------------------
class OpenGLDevice : public GPUDevice
{
public:
    OpenGLDevice();
    ~OpenGLDevice();

private:
    void __Initialize(GPUDeviceDescription* deviceDesc);
    void __Terminate();
    void __OnResize(unsigned int width, unsigned int height);

    // Shader.
    ShaderHandle* __CreateShader(Shader* shader);
    void __DeleteShader(Shader* shader);

    // Program.
    ShaderProgramHandle* __CreateProgram(ShaderProgram* program);
    void __DeleteProgram(ShaderProgram* program);
    void __EnableProgram(ShaderProgram* program);
    void __DisableProgram(ShaderProgram* program);
    void __SetProgramParameterInt(ShaderProgram* program, GLenum pname, 
        int value);

    // Uniform.
    void __GetUniformLocation(ShaderProgram* program, ShaderUniform* uniform, 
        const char* name);
    void __SetUniformValueMat4(ShaderUniform* uniform, const glm::mat4* value);
    void __SetUniformValueVec3(ShaderUniform* uniform, const glm::vec3* value);
    void __SetUniformValueInt(ShaderUniform* uniform, int value);
    void __SetUniformValueFloat(ShaderUniform* uniform, float value);
    void __SetUniformValueFloat2(ShaderUniform* uniform, const float* value);

private:
    bool mEnable4xMsaa;
    UINT m4xMsaaQuality;


};

}

#endif