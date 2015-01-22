//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "OpenGLDevice.h"
#include "Terminal.h"
#include "ShaderProgram.h"

using namespace RTGI;

GLenum gsShaderProgramParams[SPP_Max] = 
{
    GL_GEOMETRY_VERTICES_OUT_EXT
};

GLenum gsTextureFormat[TF_Max] =
{
    GL_R,
    GL_RG,
    GL_RGB,
    GL_RGBA,
    GL_RGB,
    GL_RGBA,
    GL_RED_INTEGER,
    GL_RED,
    GL_DEPTH_COMPONENT
};

GLint gsTextureInternalFormat[TIF_Max] =
{
    GL_RGB8,
    GL_RGBA8,
    GL_RGB32F_ARB,
    GL_RGBA32F_ARB,
    GL_RGBA32UI,
    GL_RGB16F_ARB,
    GL_RGBA16F_ARB,
    GL_R32UI,
    GL_R32F,
    GL_RG32F,
    GL_DEPTH_COMPONENT24
};

GLenum gsTextureComponentType[TCT_Max] =
{
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_INT,
    GL_FLOAT
};

GLenum gsBufferAccess[BA_Max] =
{
    GL_READ_ONLY,
    GL_WRITE_ONLY,
    GL_READ_WRITE
};

GLenum gsTextureTargets[TT_Max] =
{
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_2D_ARRAY
};

GLenum gsFilterType[FT_Max] =
{
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR
};

GLenum gsWrapType[WT_Max] =
{
    GL_CLAMP,
    GL_REPEAT
};

//----------------------------------------------------------------------------
void OpenGLDevice::__Initialize(GPUDeviceDescription* deviceDesc)
{
    // TODO:
    //  Should create OpenGL context here. Nothing to do now since we are
    // using GLFW.
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Terminate()
{
    // TODO:
    //  Should destroy OpenGL context here. Nothing to do now since we are
    // using GLFW.
}
//----------------------------------------------------------------------------
void OpenGLDevice::__OnResize(unsigned int width, unsigned int height)
{
    // TODO:
}
//----------------------------------------------------------------------------
ShaderHandle* OpenGLDevice::__CreateShader(Shader* shader)
{
    OpenGLShaderHandle* shaderHandle = new OpenGLShaderHandle();
    shaderHandle->Device = this;

    const char* programText = shader->GetSource().c_str();
    GLenum shaderType = shader->GetType();
    const std::string fileName = shader->GetShaderFileName();

    shaderHandle->mShader = glCreateShader(shaderType);
    glShaderSource(shaderHandle->mShader, 1, &programText, 0);
    glCompileShader(shaderHandle->mShader);

    GLint compiled;
    glGetShaderiv(shaderHandle->mShader, GL_COMPILE_STATUS, &compiled);
    if( !compiled )
    {
        GLint iInfoLen = 0;
        glGetShaderiv(shaderHandle->mShader, GL_INFO_LOG_LENGTH, &iInfoLen);
        if( iInfoLen > 1 )
        {
            char* acInfoLog = new char[iInfoLen];
            glGetShaderInfoLog(shaderHandle->mShader, iInfoLen, 0, acInfoLog);
            Terminal::Output(Terminal::OC_Error, "Failed compiling %s\n%s\n", 
                fileName.c_str(), acInfoLog);

            delete[] acInfoLog;
        }

        assert(false);
        delete  shaderHandle;
        return 0;
    }

#ifdef RTGI_OUTPUT_SHADER_RESOURCE_LOADING
    Terminal::Output(Terminal::OC_Success, "Loading shader %s finished\n", 
        fileName.c_str());
#endif

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

    return shaderHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DeleteShader(Shader* shader)
{
    OpenGLShaderHandle* shaderHandle = 
        (OpenGLShaderHandle*)shader->GetShaderHandle();
    glDeleteShader(shaderHandle->mShader);
}
//----------------------------------------------------------------------------
ShaderProgramHandle* OpenGLDevice::__CreateProgram(ShaderProgram* program)
{
    GLuint __program = glCreateProgram();
    std::string linkingString("Linking program");

    VertexShader* vShader = program->GetVertexShader();
    if( vShader )
    {
        vShader->CreateDeviceResource(this);
        OpenGLShaderHandle* vsHandle = 
            (OpenGLShaderHandle*)vShader->GetShaderHandle();
        glAttachShader(__program, vsHandle->mShader);
        linkingString += " ";
        linkingString += vShader->GetShaderFileName();
    }

    FragmentShader* fShader = program->GetFragmentShader();
    if( fShader )
    {
        fShader->CreateDeviceResource(this);
        OpenGLShaderHandle* fsHandle =
            (OpenGLShaderHandle*)fShader->GetShaderHandle();
        glAttachShader(__program, fsHandle->mShader);
        linkingString += " ";
        linkingString += fShader->GetShaderFileName();
    }

    GeometryShader* gShader = program->GetGeometryShader();
    if( gShader )
    {
        gShader->CreateDeviceResource(this);
        OpenGLShaderHandle* gsHandle =
            (OpenGLShaderHandle*)gShader->GetShaderHandle();
        glAttachShader(__program, gsHandle->mShader);
        linkingString += " ";
        linkingString += gShader->GetShaderFileName();
    }

    ComputeShader* cShader = program->GetComputeShader();
    if( cShader )
    {
        cShader->CreateDeviceResource(this);
        OpenGLShaderHandle* csHandle =
            (OpenGLShaderHandle*)cShader->GetShaderHandle();
        glAttachShader(__program, csHandle->mShader);
        linkingString += " ";
        linkingString += cShader->GetShaderFileName();
    }

    TessellationControlShader* tcShader = program->GetTessellationControlShader();
    if( tcShader )
    {
        tcShader->CreateDeviceResource(this);
        OpenGLShaderHandle* tcsHandle =
            (OpenGLShaderHandle*)tcShader->GetShaderHandle();
        glAttachShader(__program, tcsHandle->mShader);
        linkingString += " ";
        linkingString += tcShader->GetShaderFileName();
    }

    TessellationEvaluationShader* teShader = program->GetTessellationEvaluationShader();
    if( teShader )
    {
        teShader->CreateDeviceResource(this);
        OpenGLShaderHandle* tesHandle =
            (OpenGLShaderHandle*)teShader->GetShaderHandle();
        glAttachShader(__program, tesHandle->mShader);
        linkingString += " ";
        linkingString += teShader->GetShaderFileName();
    }

    glLinkProgram(__program);
    GLint linked;
    glGetProgramiv(__program, GL_LINK_STATUS, &linked);
    if( !linked )
    {
        GLint iInfoLen = 0;
        glGetProgramiv(__program, GL_INFO_LOG_LENGTH, &iInfoLen);
        if( iInfoLen > 1 )
        {
            char* acInfoLog = new char[iInfoLen];
            glGetProgramInfoLog(__program, iInfoLen, 0, acInfoLog);
            linkingString += " failed";

            Terminal::Output(Terminal::OC_Error, "%s\n%s\n", 
                linkingString.c_str(), acInfoLog);
            delete[] acInfoLog;
        }

        assert(false);
        return 0;
    }

#ifdef RTGI_OUTPUT_SHADER_RESOURCE_LOADING
    linkingString += " finished";
    Terminal::Output(Terminal::OC_Success, "%s\n", linkingString.c_str());
#endif

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

    OpenGLShaderProgramHandle* programHandle = new OpenGLShaderProgramHandle();
    programHandle->Device = this;
    programHandle->mProgram = __program;
    return programHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DeleteProgram(ShaderProgram* program)
{
    OpenGLShaderProgramHandle* programHandle =
        (OpenGLShaderProgramHandle*)program->GetProgramHandle();
    glDeleteProgram(programHandle->mProgram);
}
//----------------------------------------------------------------------------
void OpenGLDevice::__EnableProgram(ShaderProgram* program)
{
    OpenGLShaderProgramHandle* programHandle =
        (OpenGLShaderProgramHandle*)program->GetProgramHandle();
    glUseProgram(programHandle->mProgram);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DisableProgram(ShaderProgram* program)
{
    glUseProgram(0);
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetProgramParameterInt(ShaderProgram* program, 
    ShaderProgramParameter pname, int value)
{
    GLenum name = gsShaderProgramParams[(int)pname];
    OpenGLShaderProgramHandle* programHandle =
        (OpenGLShaderProgramHandle*)program->GetProgramHandle();
    glProgramParameteri(programHandle->mProgram, name, value);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__GetUniformLocation(ShaderProgram* program, 
    ShaderUniform* uniform, const char* name)
{
    OpenGLShaderProgramHandle* programHandle = 
        (OpenGLShaderProgramHandle*)program->GetProgramHandle();
    assert(programHandle);

    GLint loc = glGetUniformLocation(programHandle->mProgram, name);
    if( !uniform->mUniformHandle )
    {
        uniform->mUniformHandle = new OpenGLShaderUniformHandle();
        uniform->mUniformHandle->Device = this;
    }
    ((OpenGLShaderUniformHandle*)uniform->mUniformHandle)->mUniform = loc;

#ifdef _DEBUG
    if( loc < 0 )
    {
        Terminal::Output(Terminal::OC_Warning_Level4, 
            "Uniform: \'%s\' not found in: \n", name);
        VertexShader* vs = program->GetVertexShader();
        if( vs )
        {
            Terminal::Output(Terminal::OC_Warning_Level4, "    %s\n",
                vs->GetShaderFileName().c_str());
        }
        FragmentShader* fs = program->GetFragmentShader();
        if( fs )
        {
            Terminal::Output(Terminal::OC_Warning_Level4, "    %s\n",
                fs->GetShaderFileName().c_str());
        }
        TessellationControlShader* tcs = program->GetTessellationControlShader();
        if( tcs )
        {
            Terminal::Output(Terminal::OC_Warning_Level4, "    %s\n",
                tcs->GetShaderFileName().c_str());
        }
        TessellationEvaluationShader* tes = program->GetTessellationEvaluationShader();
        if( tes )
        {
            Terminal::Output(Terminal::OC_Warning_Level4, "    %s\n",
                tes->GetShaderFileName().c_str());
        }
        GeometryShader* gs = program->GetGeometryShader();
        if( gs )
        {
            Terminal::Output(Terminal::OC_Warning_Level4, "    %s\n",
                gs->GetShaderFileName().c_str());
        }
        ComputeShader* cs = program->GetComputeShader();
        if( cs )
        {
            Terminal::Output(Terminal::OC_Warning_Level4, "    %s\n",
                cs->GetShaderFileName().c_str());
        }
    }

    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueMat4(ShaderUniform* uniform, 
    const glm::mat4* value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    assert(uniformHandle);

    glUniformMatrix4fv(uniformHandle->mUniform, 1, GL_FALSE, glm::value_ptr(*value));

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueVec3(ShaderUniform* uniform, 
    const glm::vec3* value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    assert(uniformHandle);

    glUniform3fv(uniformHandle->mUniform, 1, (GLfloat*)value);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueInt(ShaderUniform* uniform, int value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    assert(uniformHandle);

    glUniform1i(uniformHandle->mUniform, value);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueFloat(ShaderUniform* uniform, float value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    assert(uniformHandle);

    glUniform1f(uniformHandle->mUniform, value);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueFloat2(ShaderUniform* uniform, 
    const float* value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    assert(uniformHandle);

    glUniform2fv(uniformHandle->mUniform, 1, value);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DeleteTexture(Texture* texture)
{
    OpenGLTextureHandle* textureHandle = 
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    if( textureHandle )
    {
        glDeleteTextures(1, &textureHandle->mTexture);
    }
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__Texture1DLoadFromSystemMemory(Texture* texture,
    TextureInternalFormat internalFormat, int width, TextureFormat format, 
    TextureComponentType type, void* pixels)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_1D, textureHandle->mTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, 
        gsTextureInternalFormat[(int)internalFormat], width, 0, 
        gsTextureFormat[(int)format], gsTextureComponentType[(int)type],
        pixels);

    glBindTexture(GL_TEXTURE_1D, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

    return textureHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Texture1DUpdateFromPixelBuffer(Texture* texture, 
    PixelBuffer* pixelBuffer)
{
    OpenGLTextureHandle* textureHandle = 
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    if( !textureHandle )
    {
        assert(false);
        return;
    }

    GLuint buffer = pixelBuffer->GetBuffer();
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
    glBindTexture(GL_TEXTURE_1D, textureHandle->mTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, 
        gsTextureInternalFormat[(int)texture->GetInternalFormat()], 
        ((Texture1D*)texture)->Width, 0, 
        gsTextureFormat[(int)texture->GetFormat()],
        gsTextureComponentType[(int)texture->GetComponentType()],
    	0);
    glBindTexture(GL_TEXTURE_1D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__TextureBindToImageUnit(Texture* texture, 
    unsigned int unit, BufferAccess access)
{
#if defined(__APPLE__)
    assert( false );
#else
    OpenGLTextureHandle* textureHandle =
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    if( !textureHandle )
    {
        assert(false);
        return;
    }

    glBindImageTexture(unit, textureHandle->mTexture, 0, GL_FALSE, 0, 
        gsBufferAccess[(int)access],
        gsTextureInternalFormat[(int)texture->GetInternalFormat()]);
    
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
    
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__TextureBindToSampler(Texture* texture, 
    unsigned int index, SamplerDesc* sampler)
{
    OpenGLTextureHandle* textureHandle = 
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    if( !textureHandle )
    {
        assert(false);
        return;
    }

    GLuint t = textureHandle->mTexture;
    TextureType type = texture->GetType();
    GLenum target = gsTextureTargets[(int)type];
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(target, t);

    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, 
        gsFilterType[(int)sampler->MinFilter]);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER,
        gsFilterType[(int)sampler->MagFilter]);
    glTexParameteri(target, GL_TEXTURE_WRAP_S,
        gsWrapType[(int)sampler->WrapS]);
    glTexParameteri(target, GL_TEXTURE_WRAP_T,
        gsWrapType[(int)sampler->WrapT]);
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Texture1DGetDataFromGPUMemory(Texture* texture, 
    void* dstData)
{
    OpenGLTextureHandle* textureHandle =
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    if( !textureHandle )
    {
        assert(false);
        return;
    }

    glBindTexture(GL_TEXTURE_1D, textureHandle->mTexture);
    glGetTexImage(GL_TEXTURE_1D, 0, 
        gsTextureFormat[(int)texture->GetFormat()],
        gsTextureComponentType[(int)texture->GetComponentType()],
        dstData);
    glBindTexture(GL_TEXTURE_1D, 0);
    
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__Texture2DLoadFromSystemMemory(Texture* texture,
    TextureInternalFormat internalFormat, int width, int height,
    TextureFormat format, TextureComponentType type, void* pixels)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_2D, textureHandle->mTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,
        gsTextureInternalFormat[(int)internalFormat], width, height, 0,
        gsTextureFormat[(int)format], gsTextureComponentType[(int)type],
        pixels);

    glBindTexture(GL_TEXTURE_2D, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

    return textureHandle;
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__Texture2DLoadFromTextureBuffer(
    Texture* texture, TextureBuffer* textureBuffer, 
    TextureInternalFormat internalFormat)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    GLuint buffer = textureBuffer->GetBuffer();
    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_BUFFER, textureHandle->mTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, 
        gsTextureInternalFormat[(int)internalFormat], buffer);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

    return textureHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Texture2DUpdateFromPixelBuffer(Texture* texture, 
    PixelBuffer* pixelBuffer)
{
    OpenGLTextureHandle* textureHandle = 
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    assert(textureHandle);

    GLuint buffer = pixelBuffer->GetBuffer();
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
    glBindTexture(GL_TEXTURE_2D, textureHandle->mTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,
        gsTextureInternalFormat[(int)texture->GetInternalFormat()], 
        ((Texture2D*)texture)->Width, ((Texture2D*)texture)->Height, 0,
        gsTextureFormat[(int)texture->GetFormat()],
        gsTextureComponentType[(int)texture->GetComponentType()], 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Texture2DGetImageData(Texture* texture, void* dstPixels)
{
    OpenGLTextureHandle* textureHandle =
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    assert(textureHandle);

    glBindTexture(GL_TEXTURE_2D, textureHandle->mTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, gsTextureFormat[(int)texture->GetFormat()],
        gsTextureComponentType[(int)texture->GetComponentType()], dstPixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__Tex2DArrayLoadFromSystemMemory(
    Texture* texture, TextureInternalFormat internalFormat, int width, 
    int height, int depth, TextureFormat format, TextureComponentType type, 
    void* pixels)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureHandle->mTexture);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, 
        gsTextureInternalFormat[(int)internalFormat], width, height, depth,
        0, gsTextureFormat[(int)format], gsTextureComponentType[(int)type], 
        pixels);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

    return textureHandle;
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__Texture3DLoadFromSystemMemory(
    Texture* texture, TextureInternalFormat internalFormat, int width, 
    int height, int depth, TextureFormat format, TextureComponentType type, 
    void* pixels)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_3D, textureHandle->mTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, 
        gsTextureInternalFormat[(int)internalFormat], width, height, depth, 0,
        gsTextureFormat[(int)format], gsTextureComponentType[(int)type], 
        pixels);

    glBindTexture(GL_TEXTURE_3D, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

    return textureHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Texture3DUpdateFromPixelBuffer(Texture* texture,
    PixelBuffer* pixelBuffer)
{
    OpenGLTextureHandle* textureHandle =
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    assert(textureHandle);

    GLuint buffer = pixelBuffer->GetBuffer();
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
    glBindTexture(GL_TEXTURE_3D, textureHandle->mTexture);
    glTexImage3D(GL_TEXTURE_3D, 0,
        gsTextureInternalFormat[(int)texture->GetInternalFormat()],
        ((Texture3D*)texture)->Width, ((Texture3D*)texture)->Height,
        ((Texture3D*)texture)->Depth, 0,
        gsTextureFormat[(int)texture->GetFormat()],
        gsTextureComponentType[(int)texture->GetComponentType()], 0);

    glBindTexture(GL_TEXTURE_3D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__TextureCubeLoadFromSystemMemory(
    Texture* texture, TextureInternalFormat internalFormat, int width, 
    int height, TextureFormat format, TextureComponentType type, 
    void* pixelsPX, void* pixelsNX, void* pixelsPY, void* pixelsNY, 
    void* pixelsPZ, void* pixelsNZ)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    GLint tempInternalFormat = gsTextureInternalFormat[(int)internalFormat];
    GLenum tempFormat = gsTextureFormat[(int)format];
    GLenum tempType = gsTextureComponentType[(int)type];

    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle->mTexture);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, tempInternalFormat, width,
        height, 0, tempFormat, tempType, pixelsPX);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, tempInternalFormat, width,
        height, 0, tempFormat, tempType, pixelsNX);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, tempInternalFormat, width,
        height, 0, tempFormat, tempType, pixelsPY);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, tempInternalFormat, width,
        height, 0, tempFormat, tempType, pixelsNY);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, tempInternalFormat, width,
        height, 0, tempFormat, tempType, pixelsPZ);
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, tempInternalFormat, width,
        height, 0, tempFormat, tempType, pixelsNZ);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

    return textureHandle;
}
//----------------------------------------------------------------------------
FBOHandle* OpenGLDevice::__CreateFrameBuffer(FrameBuffer* frameBuffer)
{
    OpenGLFBOHandle* fboHandle = new OpenGLFBOHandle();
    fboHandle->Device = this;

    glGenFramebuffersEXT(1, &fboHandle->mFBO);

    return fboHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DeleteFrameBuffer(FrameBuffer* frameBuffer)
{
    OpenGLFBOHandle* fboHandle = 
        (OpenGLFBOHandle*)frameBuffer->GetFBOHandle();
    assert(fboHandle);

    glDeleteFramebuffersEXT(1, &fboHandle->mFBO);
}
//----------------------------------------------------------------------------
void OpenGLDevice::__FrameBufferSetRenderTargets(FrameBuffer* frameBuffer,
    unsigned int colorTextureCount, Texture** colorTextures, 
    Texture* depthTexture, Texture* stencilTexture)
{
    OpenGLFBOHandle* fboHandle =
        (OpenGLFBOHandle*)frameBuffer->GetFBOHandle();
    assert(fboHandle);

    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle->mFBO);

    for( unsigned int i = 0; i < colorTextureCount; ++i )
    {
        OpenGLTextureHandle* textureHandle = 
            (OpenGLTextureHandle*)colorTextures[i]->GetTextureHandle();
        assert(textureHandle);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
            textureHandle->mTexture, 0);
        frameBuffer->mColorBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        frameBuffer->mColorTextures.push_back(colorTextures[i]);
    }

    if( depthTexture )
    {
        OpenGLTextureHandle* textureHandle = 
            (OpenGLTextureHandle*)depthTexture->GetTextureHandle();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
            textureHandle->mTexture, 0);
    }
    frameBuffer->mDepthTexture = depthTexture;

    if( stencilTexture )
    {
        OpenGLTextureHandle* textureHandle = 
            (OpenGLTextureHandle*)stencilTexture->GetTextureHandle();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 
            textureHandle->mTexture, 0);
    }
    frameBuffer->mStencilTexture = stencilTexture;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
static GLint oldViewport[4];
//----------------------------------------------------------------------------
void OpenGLDevice::__FrameBufferEnable(FrameBuffer* frameBuffer)
{
    // Cache old viewport values and set new values.
    glGetIntegerv(GL_VIEWPORT, oldViewport);
    glViewport(0, 0, frameBuffer->mWidth, frameBuffer->mHeight);

    OpenGLFBOHandle* fboHandle =
        (OpenGLFBOHandle*)frameBuffer->GetFBOHandle();
    assert(fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle->mFBO);
    glDrawBuffers(frameBuffer->mColorTextureCount, frameBuffer->mColorBuffers);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__FrameBufferDisable(FrameBuffer* frameBuffer)
{
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], 
        oldViewport[3]);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
OpenGLDevice::OpenGLDevice()
{
    Initialize = (GPUDeviceInitialize)&OpenGLDevice::__Initialize;
    Terminate = (GPUDeviceTerminate)&OpenGLDevice::__Terminate;
    CreateShader = (GPUDeviceCreateShader)&OpenGLDevice::__CreateShader;
    DeleteShader = (GPUDeviceDeleteShader)&OpenGLDevice::__DeleteShader;
    CreateProgram = (GPUDeviceCreateProgram)&OpenGLDevice::__CreateProgram;
    DeleteProgram = (GPUDeviceDeleteProgram)&OpenGLDevice::__DeleteProgram;
    EnableProgram = (GPUDeviceEnableProgram)&OpenGLDevice::__EnableProgram;
    DisableProgram = (GPUDeviceDisableProgram)&OpenGLDevice::__DisableProgram;
    SetProgramParameterInt = (GPUDeviceSetProgramParameterInt)&OpenGLDevice::__SetProgramParameterInt;
    GetUniformLocation = (GPUDeviceGetUniformLocation)&OpenGLDevice::__GetUniformLocation;
    SetUniformValueMat4 = (GPUDeviceSetUniformValueMat4)&OpenGLDevice::__SetUniformValueMat4;
    SetUniformValueVec3 = (GPUDeviceSetUniformValueVec3)&OpenGLDevice::__SetUniformValueVec3;
    SetUniformValueInt = (GPUDeviceSetUniformValueInt)&OpenGLDevice::__SetUniformValueInt;
    SetUniformValueFloat = (GPUDeviceSetUniformValueFloat)&OpenGLDevice::__SetUniformValueFloat;
    SetUniformValueFloat2 = (GPUDeviceSetUniformValueFloat2)&OpenGLDevice::__SetUniformValueFloat2;
    DeleteTexture = (GPUDeviceDeleteTexture)&OpenGLDevice::__DeleteTexture;
    Texture1DLoadFromSystemMemory = (GPUDeviceTexture1DLoadFromSystemMemory)&OpenGLDevice::__Texture1DLoadFromSystemMemory;
    Texture1DUpdateFromPixelBuffer = (GPUDeviceTexture1DUpdateFromPixelBuffer)&OpenGLDevice::__Texture1DUpdateFromPixelBuffer;
    TextureBindToImageUnit = (GPUDeviceTextureBindToImageUnit)&OpenGLDevice::__TextureBindToImageUnit;
    TextureBindToSampler = (GPUDeviceTextureBindToSampler)&OpenGLDevice::__TextureBindToSampler;
    Texture1DGetDataFromGPUMemory = (GPUDeviceTexture1DGetDataFromGPUMemory)&OpenGLDevice::__Texture1DGetDataFromGPUMemory;
    Texture2DLoadFromSystemMemory = (GPUDeviceTexture2DLoadFromSystemMemory)&OpenGLDevice::__Texture2DLoadFromSystemMemory;
    Texture2DLoadFromTextureBuffer = (GPUDeviceTexture2DLoadFromTextureBuffer)&OpenGLDevice::__Texture2DLoadFromTextureBuffer;
    Texture2DUpdateFromPixelBuffer = (GPUDeviceTexture2DUpdateFromPixelBuffer)&OpenGLDevice::__Texture2DUpdateFromPixelBuffer;
    Texture2DGetImageData = (GPUDeviceTexture2DGetImageData)&OpenGLDevice::__Texture2DGetImageData;
    Tex2DArrayLoadFromSystemMemory = (GPUDeviceTex2DArrayLoadFromSystemMemory)&OpenGLDevice::__Tex2DArrayLoadFromSystemMemory;
    Texture3DLoadFromSystemMemory = (GPUDeviceTexture3DLoadFromSystemMemory)&OpenGLDevice::__Texture3DLoadFromSystemMemory;
    Texture3DUpdateFromPixelBuffer = (GPUDeviceTexture3DUpdateFromPixelBuffer)&OpenGLDevice::__Texture3DUpdateFromPixelBuffer;
    TextureCubeLoadFromSystemMemory = (GPUDeviceTextureCubeLoadFromSystemMemory)&OpenGLDevice::__TextureCubeLoadFromSystemMemory;
    CreateFrameBuffer = (GPUDeviceCreateFrameBuffer)&OpenGLDevice::__CreateFrameBuffer;
    DeleteFrameBuffer = (GPUDeviceDeleteFrameBuffer)&OpenGLDevice::__DeleteFrameBuffer;
    FrameBufferSetRenderTargets = (GPUDeviceFrameBufferSetRenderTargets)&OpenGLDevice::__FrameBufferSetRenderTargets;
    FrameBufferEnable = (GPUDeviceFrameBufferEnable)&OpenGLDevice::__FrameBufferEnable;
    FrameBufferDisable = (GPUDeviceFrameBufferDisable)&OpenGLDevice::__FrameBufferDisable;

    mEnable4xMsaa = false;
    m4xMsaaQuality = 0;
}
//----------------------------------------------------------------------------
OpenGLDevice::~OpenGLDevice()
{
}
//----------------------------------------------------------------------------