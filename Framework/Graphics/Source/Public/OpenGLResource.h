//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_OpenGLResource_H
#define RTGI_OpenGLResource_H

#include "GPUResource.h"

namespace RTGI
{

struct OpenGLShaderHandle : public ShaderHandle
{
    GLuint mShader;
};

struct OpenGLShaderProgramHandle : public ShaderProgramHandle
{
    GLuint mProgram;
};

struct OpenGLPassInfoHandle : public PassInfoHandle
{
    GLuint mVAO;
};

struct OpenGLShaderUniformHandle : public ShaderUniformHandle
{
    GLint mUniform;
};

struct OpenGLTextureHandle : public TextureHandle
{
    GLuint mTexture;
};

struct OpenGLFBOHandle : public FBOHandle
{
    GLuint mFBO;
};

}

#endif