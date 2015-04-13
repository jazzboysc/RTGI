//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "OpenGLDevice.h"
#include "Terminal.h"
#include "ShaderProgram.h"
#include <GL/glew.h>

using namespace RTGI;

GLenum gsShaderProgramParams[SPP_Max] = 
{
    GL_GEOMETRY_VERTICES_OUT_EXT
};

GLenum gsBufferTargets[BufferType_Max] =
{
    GL_ATOMIC_COUNTER_BUFFER,
    GL_DISPATCH_INDIRECT_BUFFER,
    GL_DRAW_INDIRECT_BUFFER,
    GL_PIXEL_UNPACK_BUFFER,
    GL_SHADER_STORAGE_BUFFER,
    GL_TEXTURE_BUFFER,
    GL_UNIFORM_BUFFER,
    GL_ARRAY_BUFFER,
    GL_ELEMENT_ARRAY_BUFFER
};

GLenum gsBufferBindings[BufferType_Max] =
{
    GL_ATOMIC_COUNTER_BUFFER_BINDING,
    GL_DISPATCH_INDIRECT_BUFFER_BINDING,
    GL_DRAW_INDIRECT_BUFFER_BINDING,
    GL_PIXEL_UNPACK_BUFFER_BINDING,
    GL_SHADER_STORAGE_BUFFER_BINDING,
    GL_TEXTURE_BUFFER_BINDING,
    GL_UNIFORM_BUFFER_BINDING,
    GL_ARRAY_BUFFER_BINDING,
    GL_ELEMENT_ARRAY_BUFFER_BINDING
};

GLenum gsBufferFormat[BufferFormat_Max] =
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

GLint gsBufferInternalFormat[BufferInternalFormat_Max] =
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

GLenum gsBufferComponentType[BufferComponentType_Max] =
{
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_INT,
    GL_FLOAT
};

GLenum gsBufferAccess[BufferAccess_Max] =
{
    GL_READ_ONLY,
    GL_WRITE_ONLY,
    GL_READ_WRITE
};

GLenum gsBufferUsage[BufferUsage_Max] =
{
    GL_STATIC_READ,
    GL_STATIC_COPY,
    GL_STATIC_DRAW,
    GL_DYNAMIC_READ,
    GL_DYNAMIC_COPY,
    GL_DYNAMIC_DRAW,
	GL_STREAM_READ,
	GL_STREAM_COPY,
	GL_STREAM_DRAW
};

GLenum gsTextureTargets[TextureType_Max] =
{
    GL_TEXTURE_1D,
    GL_TEXTURE_2D,
    GL_TEXTURE_3D,
    GL_TEXTURE_CUBE_MAP,
    GL_TEXTURE_2D_ARRAY,
    GL_TEXTURE_BUFFER
};

GLenum gsFilterType[FilterType_Max] =
{
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR
};

GLenum gsWrapType[WrapType_Max] =
{
    GL_CLAMP,
    GL_REPEAT
};

#ifdef _DEBUG
#define OPENGL_DEVICE_CHECK_ERROR  \
    {  \
        GLenum res = glGetError();  \
        RTGI_ASSERT(res == GL_NO_ERROR);  \
    }
#else
#define OPENGL_DEVICE_CHECK_ERROR
#endif

//----------------------------------------------------------------------------
void OpenGLDevice::__Initialize(GPUDeviceDescription* deviceDesc)
{
    // TODO:
    //  Should create OpenGL context here. Nothing to do now since we are
    // using GLFW.

    // Default global states.
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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

        RTGI_ASSERT(false);
        delete  shaderHandle;
        return 0;
    }

#ifdef RTGI_OUTPUT_SHADER_RESOURCE_LOADING
    Terminal::Output(Terminal::OC_Success, "Loading shader %s finished\n", 
        fileName.c_str());
#endif

    OPENGL_DEVICE_CHECK_ERROR;

    return shaderHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DeleteShader(Shader* shader)
{
    OpenGLShaderHandle* shaderHandle = 
        (OpenGLShaderHandle*)shader->GetShaderHandle();
    glDeleteShader(shaderHandle->mShader);

    OPENGL_DEVICE_CHECK_ERROR;
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

        RTGI_ASSERT(false);
        return 0;
    }

#ifdef RTGI_OUTPUT_SHADER_RESOURCE_LOADING
    linkingString += " finished";
    Terminal::Output(Terminal::OC_Success, "%s\n", linkingString.c_str());
#endif

    OPENGL_DEVICE_CHECK_ERROR;

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

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__EnableProgram(ShaderProgram* program)
{
    OpenGLShaderProgramHandle* programHandle =
        (OpenGLShaderProgramHandle*)program->GetProgramHandle();
    glUseProgram(programHandle->mProgram);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DisableProgram(ShaderProgram* program)
{
    glUseProgram(0);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetProgramParameterInt(ShaderProgram* program, 
    ShaderProgramParameter pname, int value)
{
    GLenum name = gsShaderProgramParams[(int)pname];
    OpenGLShaderProgramHandle* programHandle =
        (OpenGLShaderProgramHandle*)program->GetProgramHandle();
    glProgramParameteri(programHandle->mProgram, name, value);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__GetUniformLocation(ShaderProgram* program, 
    ShaderUniform* uniform, const char* name)
{
    OpenGLShaderProgramHandle* programHandle = 
        (OpenGLShaderProgramHandle*)program->GetProgramHandle();
    RTGI_ASSERT(programHandle);

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
#endif

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueMat4(ShaderUniform* uniform, 
    const glm::mat4* value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    RTGI_ASSERT(uniformHandle);

    glUniformMatrix4fv(uniformHandle->mUniform, 1, GL_FALSE, glm::value_ptr(*value));

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueVec3(ShaderUniform* uniform, 
    const glm::vec3* value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    RTGI_ASSERT(uniformHandle);

    glUniform3fv(uniformHandle->mUniform, 1, (GLfloat*)value);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueInt(ShaderUniform* uniform, int value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    RTGI_ASSERT(uniformHandle);

    glUniform1i(uniformHandle->mUniform, value);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueUInt(ShaderUniform* uniform, 
    unsigned int value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    RTGI_ASSERT(uniformHandle);

    glUniform1ui(uniformHandle->mUniform, value);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueFloat(ShaderUniform* uniform, float value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    RTGI_ASSERT(uniformHandle);

    glUniform1f(uniformHandle->mUniform, value);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueFloat2(ShaderUniform* uniform, 
    const float* value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    RTGI_ASSERT(uniformHandle);

    glUniform2fv(uniformHandle->mUniform, 1, value);

    OPENGL_DEVICE_CHECK_ERROR;
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

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__Texture1DLoadFromSystemMemory(Texture* texture,
    BufferInternalFormat internalFormat, int width, BufferFormat format, 
    BufferComponentType type, void* pixels)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_1D, textureHandle->mTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, 
        gsBufferInternalFormat[(int)internalFormat], width, 0, 
        gsBufferFormat[(int)format], gsBufferComponentType[(int)type],
        pixels);

    glBindTexture(GL_TEXTURE_1D, 0);

    OPENGL_DEVICE_CHECK_ERROR;

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
        RTGI_ASSERT(false);
        return;
    }

    GLuint buffer = 
        ((OpenGLBufferHandle*)pixelBuffer->GetBufferHandle())->mBuffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
    glBindTexture(GL_TEXTURE_1D, textureHandle->mTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, 
        gsBufferInternalFormat[(int)texture->GetInternalFormat()], 
        ((Texture1D*)texture)->Width, 0, 
        gsBufferFormat[(int)texture->GetFormat()],
        gsBufferComponentType[(int)texture->GetComponentType()],
    	0);
    glBindTexture(GL_TEXTURE_1D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    
    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__TextureBindToImageUnit(Texture* texture, 
	unsigned int unit, bool layered, BufferAccess access)
{
#if defined(__APPLE__)
    RTGI_ASSERT( false );
#else
    OpenGLTextureHandle* textureHandle =
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    if( !textureHandle )
    {
        RTGI_ASSERT(false);
        return;
    }

	glBindImageTexture(unit, textureHandle->mTexture, 0, layered, 0,
        gsBufferAccess[(int)access],
        gsBufferInternalFormat[(int)texture->GetInternalFormat()]);
    
    OPENGL_DEVICE_CHECK_ERROR;
    
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
        RTGI_ASSERT(false);
        return;
    }

    // Sampler binding.
    GLuint t = textureHandle->mTexture;
    TextureType type = texture->GetType();
    GLenum target = gsTextureTargets[(int)type];
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(target, t);

    OPENGL_DEVICE_CHECK_ERROR;

	// Anisotropic Filtering.
	if( mAnisFilterLevel )
	{
		glTexParameteri(target, GL_TEXTURE_MAX_ANISOTROPY_EXT, 
            mAnisFilterLevel);
	}

    OPENGL_DEVICE_CHECK_ERROR;

    if( sampler )
    {
        // Filtering.
        glTexParameteri(target, GL_TEXTURE_MIN_FILTER,
            gsFilterType[(int)sampler->MinFilter]);

        OPENGL_DEVICE_CHECK_ERROR;

        glTexParameteri(target, GL_TEXTURE_MAG_FILTER,
            gsFilterType[(int)sampler->MagFilter]);

        OPENGL_DEVICE_CHECK_ERROR;

        // Texture coordinates wrapping.
        glTexParameteri(target, GL_TEXTURE_WRAP_S,
            gsWrapType[(int)sampler->WrapS]);

        OPENGL_DEVICE_CHECK_ERROR;

        glTexParameteri(target, GL_TEXTURE_WRAP_T,
            gsWrapType[(int)sampler->WrapT]);

        OPENGL_DEVICE_CHECK_ERROR;

        if( target == GL_TEXTURE_CUBE_MAP )
        {
            glTexParameteri(target, GL_TEXTURE_WRAP_R,
                gsWrapType[(int)sampler->WrapR]);

            OPENGL_DEVICE_CHECK_ERROR;
        }
    }
}
//----------------------------------------------------------------------------
void OpenGLDevice::__TextureGenerateMipmap(Texture* texture)
{
    OpenGLTextureHandle* textureHandle =
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    RTGI_ASSERT(textureHandle);

    TextureType type = texture->GetType();
    GLenum target = gsTextureTargets[(int)type];
    glBindTexture(target, textureHandle->mTexture);
    glGenerateMipmap(target);
    glBindTexture(target, 0);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Texture1DGetDataFromGPUMemory(Texture* texture, 
    void* dstData)
{
    OpenGLTextureHandle* textureHandle =
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    if( !textureHandle )
    {
        RTGI_ASSERT(false);
        return;
    }

    glBindTexture(GL_TEXTURE_1D, textureHandle->mTexture);
    glGetTexImage(GL_TEXTURE_1D, 0, 
        gsBufferFormat[(int)texture->GetFormat()],
        gsBufferComponentType[(int)texture->GetComponentType()],
        dstData);
    glBindTexture(GL_TEXTURE_1D, 0);
    
    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__Texture2DLoadFromSystemMemory(Texture* texture,
    BufferInternalFormat internalFormat, int width, int height,
    BufferFormat format, BufferComponentType type, bool mipMap, 
    SamplerDesc* sampler, void* pixels)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_2D, textureHandle->mTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,
        gsBufferInternalFormat[(int)internalFormat], width, height, 0,
        gsBufferFormat[(int)format], gsBufferComponentType[(int)type],
        pixels);

    if( mipMap )
    {
        if( sampler )
        {
            // Mipmap filtering.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                gsFilterType[(int)sampler->MinFilter]);

            OPENGL_DEVICE_CHECK_ERROR;

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                gsFilterType[(int)sampler->MagFilter]);

            OPENGL_DEVICE_CHECK_ERROR;

            // Texture coordinates wrapping.
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                gsWrapType[(int)sampler->WrapS]);

            OPENGL_DEVICE_CHECK_ERROR;

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                gsWrapType[(int)sampler->WrapT]);

            OPENGL_DEVICE_CHECK_ERROR;
        }

        glGenerateMipmap(GL_TEXTURE_2D);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    OPENGL_DEVICE_CHECK_ERROR;

    return textureHandle;
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__Texture2DLoadFromTextureBuffer(
    Texture* texture, TextureBuffer* textureBuffer, 
    BufferInternalFormat internalFormat)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    GLuint buffer = 
        ((OpenGLBufferHandle*)textureBuffer->GetBufferHandle())->mBuffer;
    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_BUFFER, textureHandle->mTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, 
        gsBufferInternalFormat[(int)internalFormat], buffer);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    OPENGL_DEVICE_CHECK_ERROR;

    return textureHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Texture2DUpdateFromPixelBuffer(Texture* texture, 
    PixelBuffer* pixelBuffer)
{
    OpenGLTextureHandle* textureHandle = 
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    RTGI_ASSERT(textureHandle);

    GLuint buffer = 
        ((OpenGLBufferHandle*)pixelBuffer->GetBufferHandle())->mBuffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
    glBindTexture(GL_TEXTURE_2D, textureHandle->mTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,
        gsBufferInternalFormat[(int)texture->GetInternalFormat()], 
        ((Texture2D*)texture)->Width, ((Texture2D*)texture)->Height, 0,
        gsBufferFormat[(int)texture->GetFormat()],
        gsBufferComponentType[(int)texture->GetComponentType()], 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    
    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Texture2DGetImageData(Texture* texture, void* dstPixels)
{
    OpenGLTextureHandle* textureHandle =
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    RTGI_ASSERT(textureHandle);

    glBindTexture(GL_TEXTURE_2D, textureHandle->mTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, gsBufferFormat[(int)texture->GetFormat()],
        gsBufferComponentType[(int)texture->GetComponentType()], dstPixels);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__Tex2DArrayLoadFromSystemMemory(
    Texture* texture, BufferInternalFormat internalFormat, int width, 
    int height, int depth, BufferFormat format, BufferComponentType type, 
	bool mipmap, void* pixels)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureHandle->mTexture);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, 
        gsBufferInternalFormat[(int)internalFormat], width, height, depth,
        0, gsBufferFormat[(int)format], gsBufferComponentType[(int)type], 
        pixels);

	if (mipmap)
	{
		glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	}

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    OPENGL_DEVICE_CHECK_ERROR;

    return textureHandle;
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__Texture3DLoadFromSystemMemory(
    Texture* texture, BufferInternalFormat internalFormat, int width, 
    int height, int depth, BufferFormat format, BufferComponentType type, 
    void* pixels)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_3D, textureHandle->mTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, 
        gsBufferInternalFormat[(int)internalFormat], width, height, depth, 0,
        gsBufferFormat[(int)format], gsBufferComponentType[(int)type], 
        pixels);

    glBindTexture(GL_TEXTURE_3D, 0);

    OPENGL_DEVICE_CHECK_ERROR;

    return textureHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Texture3DUpdateFromPixelBuffer(Texture* texture,
    PixelBuffer* pixelBuffer)
{
    OpenGLTextureHandle* textureHandle =
        (OpenGLTextureHandle*)texture->GetTextureHandle();
    RTGI_ASSERT(textureHandle);

    GLuint buffer = 
        ((OpenGLBufferHandle*)pixelBuffer->GetBufferHandle())->mBuffer;
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
    glBindTexture(GL_TEXTURE_3D, textureHandle->mTexture);
    glTexImage3D(GL_TEXTURE_3D, 0,
        gsBufferInternalFormat[(int)texture->GetInternalFormat()],
        ((Texture3D*)texture)->Width, ((Texture3D*)texture)->Height,
        ((Texture3D*)texture)->Depth, 0,
        gsBufferFormat[(int)texture->GetFormat()],
        gsBufferComponentType[(int)texture->GetComponentType()], 0);

    glBindTexture(GL_TEXTURE_3D, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
    
    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__TextureCubeLoadFromSystemMemory(
    Texture* texture, BufferInternalFormat internalFormat, int width, 
    int height, BufferFormat format, BufferComponentType type, bool mipMap, 
    void* pixelsPX, void* pixelsNX, void* pixelsPY, void* pixelsNY, 
    void* pixelsPZ, void* pixelsNZ)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    GLint tempInternalFormat = gsBufferInternalFormat[(int)internalFormat];
    GLenum tempFormat = gsBufferFormat[(int)format];
    GLenum tempType = gsBufferComponentType[(int)type];

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

    if( mipMap )
    {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    OPENGL_DEVICE_CHECK_ERROR;

    return textureHandle;
}
//----------------------------------------------------------------------------
TextureHandle* OpenGLDevice::__BufferTextureLoadFromTextureBuffer(
    Texture* texture, TextureBuffer* textureBuffer, 
    BufferInternalFormat internalFormat)
{
    OpenGLTextureHandle* textureHandle = new OpenGLTextureHandle();
    textureHandle->Device = this;

    GLuint buffer =
        ((OpenGLBufferHandle*)textureBuffer->GetBufferHandle())->mBuffer;
    glGenTextures(1, &textureHandle->mTexture);
    glBindTexture(GL_TEXTURE_BUFFER, textureHandle->mTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, 
        gsBufferInternalFormat[(int)internalFormat], buffer);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

    OPENGL_DEVICE_CHECK_ERROR;

    return textureHandle;
}
//----------------------------------------------------------------------------
FBOHandle* OpenGLDevice::__CreateFrameBuffer(FrameBuffer* frameBuffer)
{
    OpenGLFBOHandle* fboHandle = new OpenGLFBOHandle();
    fboHandle->Device = this;

    glGenFramebuffersEXT(1, &fboHandle->mFBO);

    OPENGL_DEVICE_CHECK_ERROR;

    return fboHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DeleteFrameBuffer(FrameBuffer* frameBuffer)
{
    OpenGLFBOHandle* fboHandle = 
        (OpenGLFBOHandle*)frameBuffer->GetFBOHandle();
    RTGI_ASSERT(fboHandle);

    glDeleteFramebuffersEXT(1, &fboHandle->mFBO);
}
//----------------------------------------------------------------------------
void OpenGLDevice::__FrameBufferSetRenderTargets(FrameBuffer* frameBuffer,
    unsigned int colorTextureCount, Texture** colorTextures, 
    Texture* depthTexture, Texture* stencilTexture)
{
    OpenGLFBOHandle* fboHandle =
        (OpenGLFBOHandle*)frameBuffer->GetFBOHandle();
    RTGI_ASSERT(fboHandle);

    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle->mFBO);

    for( unsigned int i = 0; i < colorTextureCount; ++i )
    {
        OpenGLTextureHandle* textureHandle = 
            (OpenGLTextureHandle*)colorTextures[i]->GetTextureHandle();
        RTGI_ASSERT(textureHandle);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
            textureHandle->mTexture, 0);
        frameBuffer->mColorBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
        frameBuffer->mColorTextures.push_back(colorTextures[i]);
    }

    OPENGL_DEVICE_CHECK_ERROR;

    if( depthTexture )
    {
        OpenGLTextureHandle* textureHandle = 
            (OpenGLTextureHandle*)depthTexture->GetTextureHandle();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
            textureHandle->mTexture, 0);
    }
    frameBuffer->mDepthTexture = depthTexture;

    OPENGL_DEVICE_CHECK_ERROR;

    if( stencilTexture )
    {
        OpenGLTextureHandle* textureHandle = 
            (OpenGLTextureHandle*)stencilTexture->GetTextureHandle();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 
            textureHandle->mTexture, 0);
    }
    frameBuffer->mStencilTexture = stencilTexture;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__FrameBufferSetColorTarget(FrameBuffer* frameBuffer, 
    unsigned int attachPoint, Texture* colorTexture)
{
    OpenGLFBOHandle* fboHandle =
        (OpenGLFBOHandle*)frameBuffer->GetFBOHandle();
    RTGI_ASSERT(fboHandle);

    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle->mFBO);

    if( colorTexture )
    {
        OpenGLTextureHandle* textureHandle =
            (OpenGLTextureHandle*)colorTexture->GetTextureHandle();
        RTGI_ASSERT(textureHandle);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachPoint,
            textureHandle->mTexture, 0);
    }
    else
    {
        glFramebufferTexture(GL_FRAMEBUFFER, 
            GL_COLOR_ATTACHMENT0 + attachPoint, 0, 0);
    }
    frameBuffer->mColorBuffers[attachPoint] =
        GL_COLOR_ATTACHMENT0 + attachPoint;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__FrameBufferSetDepthTarget(FrameBuffer* frameBuffer, 
    Texture* depthTexture)
{
    OpenGLFBOHandle* fboHandle =
        (OpenGLFBOHandle*)frameBuffer->GetFBOHandle();
    RTGI_ASSERT(fboHandle);

    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle->mFBO);

    if( depthTexture )
    {
        OpenGLTextureHandle* textureHandle =
            (OpenGLTextureHandle*)depthTexture->GetTextureHandle();
        RTGI_ASSERT(textureHandle);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
            textureHandle->mTexture, 0);
    }
    else
    {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 0, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__FrameBufferSetStencilTarget(FrameBuffer* frameBuffer, 
    Texture* stencilTexture)
{
    OpenGLFBOHandle* fboHandle =
        (OpenGLFBOHandle*)frameBuffer->GetFBOHandle();
    RTGI_ASSERT(fboHandle);

    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle->mFBO);

    if( stencilTexture )
    {
        OpenGLTextureHandle* textureHandle =
            (OpenGLTextureHandle*)stencilTexture->GetTextureHandle();
        RTGI_ASSERT(textureHandle);

        glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT,
            textureHandle->mTexture, 0);
    }
    else
    {
        glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, 0, 0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    OPENGL_DEVICE_CHECK_ERROR;
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
    RTGI_ASSERT(fboHandle);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle->mFBO);
    glDrawBuffers(frameBuffer->mColorTextureCount, frameBuffer->mColorBuffers);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__FrameBufferDisable(FrameBuffer* frameBuffer)
{
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], 
        oldViewport[3]);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__ComputeShaderDispatch(ShaderProgram* program, 
    unsigned int globalX, unsigned int globalY, unsigned int globalZ)
{
    glDispatchCompute(globalX, globalY, globalZ);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DispatchVertex(unsigned int threadCount)
{
    glDrawArrays(GL_POINTS, 0, threadCount);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DispatchVertexIndirect(void* indirect)
{
    glDrawArraysIndirect(GL_POINTS, indirect);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DeleteBuffer(Buffer* buffer)
{
    OpenGLBufferHandle* bufferHandle = 
        (OpenGLBufferHandle*)buffer->GetBufferHandle();
    RTGI_ASSERT(bufferHandle);
    glDeleteBuffers(1, &bufferHandle->mBuffer);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void* OpenGLDevice::__BufferMap(Buffer* buffer, BufferAccess access)
{
#ifdef _DEBUG
    OpenGLBufferHandle* bufferHandle =
        (OpenGLBufferHandle*)buffer->GetBufferHandle();
    RTGI_ASSERT(bufferHandle);

    GLint curBufferBound;
    glGetIntegerv(gsBufferBindings[(int)buffer->GetType()], &curBufferBound);
    RTGI_ASSERT(bufferHandle->mBuffer == curBufferBound);
#endif

    void* data = glMapBuffer(gsBufferTargets[(int)buffer->GetType()], 
        gsBufferAccess[(int)access]);

    OPENGL_DEVICE_CHECK_ERROR;

    return data;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__BufferUnmap(Buffer* buffer)
{
    glUnmapBuffer(gsBufferTargets[(int)buffer->GetType()]);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__BufferBindIndex(Buffer* buffer, unsigned int index)
{
#ifndef __APPLE__

    GLuint b = ((OpenGLBufferHandle*)buffer->GetBufferHandle())->mBuffer;
    glBindBufferBase(gsBufferTargets[(int)buffer->GetType()], index, b);
    
    OPENGL_DEVICE_CHECK_ERROR;

#else
    RTGI_ASSERT(false);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__BufferBindIndexTo(Buffer* buffer, unsigned int index, 
    BufferView* view)
{
#ifndef __APPLE__

    GLuint b = ((OpenGLBufferHandle*)buffer->GetBufferHandle())->mBuffer;
    glBindBufferBase(gsBufferTargets[(int)view->GetBufferType()], index, b);

    OPENGL_DEVICE_CHECK_ERROR;

#else
    RTGI_ASSERT(false);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__BufferBind(Buffer* buffer)
{
    GLuint b = ((OpenGLBufferHandle*)buffer->GetBufferHandle())->mBuffer;
    GLenum target = gsBufferTargets[(int)buffer->GetType()];
    glBindBuffer(target, b);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__BufferBindTo(Buffer* buffer, BufferView* view)
{
    GLuint b = ((OpenGLBufferHandle*)buffer->GetBufferHandle())->mBuffer;
    glBindBuffer(gsBufferTargets[(int)view->GetBufferType()], b);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
BufferViewHandle* OpenGLDevice::__CreateBufferView(BufferView* view)
{
    OpenGLBufferViewHandle* bufferViewHandle = new OpenGLBufferViewHandle();
    bufferViewHandle->Device = this;
    return bufferViewHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__DeleteBufferView(BufferView*)
{
    // Nothing to do.
}
//----------------------------------------------------------------------------
void OpenGLDevice::__BufferUpdateSubData(Buffer* buffer, int offset, 
    size_t size, void* data)
{
    glBufferSubData(gsBufferTargets[(int)buffer->GetType()], offset, size, 
        data);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------
BufferHandle* OpenGLDevice::__BufferLoadFromSystemMemory(Buffer* buffer, 
    size_t size, void* data, BufferUsage usage)
{
    OpenGLBufferHandle* bufferHandle = new OpenGLBufferHandle();
    bufferHandle->Device = this;

    GLenum type = gsBufferTargets[(int)buffer->GetType()];
    glGenBuffers(1, &bufferHandle->mBuffer);
    glBindBuffer(type, bufferHandle->mBuffer);
    glBufferData(type, size, data, gsBufferUsage[(int)usage]);
    glBindBuffer(type, 0);

    OPENGL_DEVICE_CHECK_ERROR;

    return bufferHandle;
}
//----------------------------------------------------------------------------
BufferHandle* OpenGLDevice::__BufferLoadImmutableFromSystemMemory(
    Buffer* buffer, size_t size, void* data)
{
    OpenGLBufferHandle* bufferHandle = new OpenGLBufferHandle();
    bufferHandle->Device = this;

    GLenum type = gsBufferTargets[(int)buffer->GetType()];
    glGenBuffers(1, &bufferHandle->mBuffer);
    glBindBuffer(type, bufferHandle->mBuffer);
    glBufferStorage(type, size, data, 0);
    glBindBuffer(type, 0);

    OPENGL_DEVICE_CHECK_ERROR;

    return bufferHandle;
}
//----------------------------------------------------------------------------
void OpenGLDevice::__BufferClear(Buffer* buffer, 
    BufferInternalFormat internalFormat, BufferFormat format, 
    BufferComponentType type, void* data)
{
    GLuint b = ((OpenGLBufferHandle*)buffer->GetBufferHandle())->mBuffer;

    glInvalidateBufferData(b);
    glClearBufferData(gsBufferTargets[(int)buffer->GetType()], 
        gsBufferInternalFormat[(int)internalFormat],
        gsBufferFormat[(int)format], gsBufferComponentType[(int)type], data);

    OPENGL_DEVICE_CHECK_ERROR;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
OpenGLDevice::OpenGLDevice()
{
    _Initialize = (GPUDeviceInitialize)&OpenGLDevice::__Initialize;
    _Terminate = (GPUDeviceTerminate)&OpenGLDevice::__Terminate;
    _CreateShader = (GPUDeviceCreateShader)&OpenGLDevice::__CreateShader;
    _DeleteShader = (GPUDeviceDeleteShader)&OpenGLDevice::__DeleteShader;
    _CreateProgram = (GPUDeviceCreateProgram)&OpenGLDevice::__CreateProgram;
    _DeleteProgram = (GPUDeviceDeleteProgram)&OpenGLDevice::__DeleteProgram;
    _EnableProgram = (GPUDeviceEnableProgram)&OpenGLDevice::__EnableProgram;
    _DisableProgram = (GPUDeviceDisableProgram)&OpenGLDevice::__DisableProgram;
    _SetProgramParameterInt = (GPUDeviceSetProgramParameterInt)&OpenGLDevice::__SetProgramParameterInt;
    _GetUniformLocation = (GPUDeviceGetUniformLocation)&OpenGLDevice::__GetUniformLocation;
    _SetUniformValueMat4 = (GPUDeviceSetUniformValueMat4)&OpenGLDevice::__SetUniformValueMat4;
    _SetUniformValueVec3 = (GPUDeviceSetUniformValueVec3)&OpenGLDevice::__SetUniformValueVec3;
    _SetUniformValueInt = (GPUDeviceSetUniformValueInt)&OpenGLDevice::__SetUniformValueInt;
    _SetUniformValueUInt = (GPUDeviceSetUniformValueUInt)&OpenGLDevice::__SetUniformValueUInt;
    _SetUniformValueFloat = (GPUDeviceSetUniformValueFloat)&OpenGLDevice::__SetUniformValueFloat;
    _SetUniformValueFloat2 = (GPUDeviceSetUniformValueFloat2)&OpenGLDevice::__SetUniformValueFloat2;
    _DeleteTexture = (GPUDeviceDeleteTexture)&OpenGLDevice::__DeleteTexture;
    _Texture1DLoadFromSystemMemory = (GPUDeviceTexture1DLoadFromSystemMemory)&OpenGLDevice::__Texture1DLoadFromSystemMemory;
    _Texture1DUpdateFromPixelBuffer = (GPUDeviceTexture1DUpdateFromPixelBuffer)&OpenGLDevice::__Texture1DUpdateFromPixelBuffer;
    _TextureBindToImageUnit = (GPUDeviceTextureBindToImageUnit)&OpenGLDevice::__TextureBindToImageUnit;
    _TextureBindToSampler = (GPUDeviceTextureBindToSampler)&OpenGLDevice::__TextureBindToSampler;
    _TextureGenerateMipmap = (GPUDeviceTextureGenerateMipmap)&OpenGLDevice::__TextureGenerateMipmap;
    _Texture1DGetDataFromGPUMemory = (GPUDeviceTexture1DGetDataFromGPUMemory)&OpenGLDevice::__Texture1DGetDataFromGPUMemory;
    _Texture2DLoadFromSystemMemory = (GPUDeviceTexture2DLoadFromSystemMemory)&OpenGLDevice::__Texture2DLoadFromSystemMemory;
    _Texture2DLoadFromTextureBuffer = (GPUDeviceTexture2DLoadFromTextureBuffer)&OpenGLDevice::__Texture2DLoadFromTextureBuffer;
    _Texture2DUpdateFromPixelBuffer = (GPUDeviceTexture2DUpdateFromPixelBuffer)&OpenGLDevice::__Texture2DUpdateFromPixelBuffer;
    _Texture2DGetImageData = (GPUDeviceTexture2DGetImageData)&OpenGLDevice::__Texture2DGetImageData;
    _Tex2DArrayLoadFromSystemMemory = (GPUDeviceTex2DArrayLoadFromSystemMemory)&OpenGLDevice::__Tex2DArrayLoadFromSystemMemory;
    _Texture3DLoadFromSystemMemory = (GPUDeviceTexture3DLoadFromSystemMemory)&OpenGLDevice::__Texture3DLoadFromSystemMemory;
    _Texture3DUpdateFromPixelBuffer = (GPUDeviceTexture3DUpdateFromPixelBuffer)&OpenGLDevice::__Texture3DUpdateFromPixelBuffer;
    _TextureCubeLoadFromSystemMemory = (GPUDeviceTextureCubeLoadFromSystemMemory)&OpenGLDevice::__TextureCubeLoadFromSystemMemory;
    _BufferTextureLoadFromTextureBuffer = (GPUDeviceBufferTextureLoadFromTextureBuffer)&OpenGLDevice::__BufferTextureLoadFromTextureBuffer;
    _CreateFrameBuffer = (GPUDeviceCreateFrameBuffer)&OpenGLDevice::__CreateFrameBuffer;
    _DeleteFrameBuffer = (GPUDeviceDeleteFrameBuffer)&OpenGLDevice::__DeleteFrameBuffer;
    _FrameBufferSetRenderTargets = (GPUDeviceFrameBufferSetRenderTargets)&OpenGLDevice::__FrameBufferSetRenderTargets;
    _FrameBufferSetColorTarget = (GPUDeviceFrameBufferSetColorTarget)&OpenGLDevice::__FrameBufferSetColorTarget;
    _FrameBufferSetDepthTarget = (GPUDeviceFrameBufferSetDepthTarget)&OpenGLDevice::__FrameBufferSetDepthTarget;
    _FrameBufferSetStencilTarget = (GPUDeviceFrameBufferSetStencilTarget)&OpenGLDevice::__FrameBufferSetStencilTarget;
    _FrameBufferEnable = (GPUDeviceFrameBufferEnable)&OpenGLDevice::__FrameBufferEnable;
    _FrameBufferDisable = (GPUDeviceFrameBufferDisable)&OpenGLDevice::__FrameBufferDisable;
    _ComputeShaderDispatch = (GPUDeviceComputeShaderDispatch)&OpenGLDevice::__ComputeShaderDispatch;
    _DispatchVertex = (GPUDeviceDispatchVertex)&OpenGLDevice::__DispatchVertex;
    _DispatchVertexIndirect = (GPUDeviceDispatchVertexIndirect)&OpenGLDevice::__DispatchVertexIndirect;
    _DeleteBuffer = (GPUDeviceDeleteBuffer)&OpenGLDevice::__DeleteBuffer;
    _BufferMap = (GPUDeviceBufferMap)&OpenGLDevice::__BufferMap;
    _BufferUnmap = (GPUDeviceBufferUnmap)&OpenGLDevice::__BufferUnmap;
    _BufferBindIndex = (GPUDeviceBufferBindIndex)&OpenGLDevice::__BufferBindIndex;
    _BufferBindIndexTo = (GPUDeviceBufferBindIndexTo)&OpenGLDevice::__BufferBindIndexTo;
    _BufferBind = (GPUDeviceBufferBind)&OpenGLDevice::__BufferBind;
    _BufferBindTo = (GPUDeviceBufferBindTo)&OpenGLDevice::__BufferBindTo;
    _CreateBufferView = (GPUDeviceCreateBufferView)&OpenGLDevice::__CreateBufferView;
    _DeleteBufferView = (GPUDeviceDeleteBufferView)&OpenGLDevice::__DeleteBufferView;
    _BufferUpdateSubData = (GPUDeviceBufferUpdateSubData)&OpenGLDevice::__BufferUpdateSubData;
    _BufferLoadFromSystemMemory = (GPUDeviceBufferLoadFromSystemMemory)&OpenGLDevice::__BufferLoadFromSystemMemory;
    _BufferLoadImmutableFromSystemMemory = (GPUDeviceBufferLoadImmutableFromSystemMemory)&OpenGLDevice::__BufferLoadImmutableFromSystemMemory;
    _BufferClear = (GPUDeviceBufferClear)&OpenGLDevice::__BufferClear;
	_GetMaxAnisFilterLevel = (GPUDeviceGetMaxAnisFilterLevel)&OpenGLDevice::__GetMaxAnisFilterLevel;
	_SetAnisFilterLevel = (GPUDeviceSetAnisFilterLevel)&OpenGLDevice::__SetAnisFilterLevel;


    mEnable4xMsaa = false;
    m4xMsaaQuality = 0;
	mAnisFilterLevel = 0;
}
//----------------------------------------------------------------------------
OpenGLDevice::~OpenGLDevice()
{
}
//----------------------------------------------------------------------------
/* From: https://www.opengl.org/archives/resources/features/OGLextensions/ */
int RTGI::OpenGLDevice::__IsExtensionSupported(const char *extension)
{
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	/* Extension names should not have spaces. */
	where = (GLubyte *)strchr(extension, ' ');
	if (where || *extension == '\0')
		return 0;
	extensions = glGetString(GL_EXTENSIONS);
	/* It takes a bit of care to be fool-proof about parsing the
	OpenGL extensions string. Don't be fooled by sub-strings,
	etc. */
	start = extensions;
	for (;;)
	{
		where = (GLubyte *)strstr((const char *)start, extension);
		if (!where)
			break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
		if (*terminator == ' ' || *terminator == '\0')
			return 1;
		start = terminator;
	}
	return 0;
}
//----------------------------------------------------------------------------

void RTGI::OpenGLDevice::__GetMaxAnisFilterLevel(int* maxAnisFilterLevel)
{
	// TODO: maybe move this somewhere else
	if (__IsExtensionSupported("GL_EXT_texture_filter_anisotropic"))
	{
		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisFilterLevel);
	}
	else
	{
		*maxAnisFilterLevel = 0;
	}
}
//----------------------------------------------------------------------------

void RTGI::OpenGLDevice::__SetAnisFilterLevel(int anisFilterLevel)
{
	GLint maxAnisFilterLevel;
	__GetMaxAnisFilterLevel(&maxAnisFilterLevel);

	if (maxAnisFilterLevel >= anisFilterLevel)
	{
		mAnisFilterLevel = anisFilterLevel;
	}
}

//----------------------------------------------------------------------------