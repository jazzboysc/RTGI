//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "OpenGLDevice.h"

using namespace RTGI;

//----------------------------------------------------------------------------
void OpenGLDevice::__Initialize(GPUDeviceDescription* deviceDesc)
{
    // TODO:
    //  Should create OpenGL context here. Nothing to do now since we are
    // using GLUT.
}
//----------------------------------------------------------------------------
void OpenGLDevice::__Terminate()
{
    // TODO:
    //  Should destroy OpenGL context here. Nothing to do now since we are
    // using GLUT.
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
            printf("Failed compiling %s\n%s\n", fileName.c_str(), acInfoLog);

            delete[] acInfoLog;
        }

        assert(false);
        delete  shaderHandle;
        return 0;
    }

#ifdef RTGI_OUTPUT_RESOURCE_LOADING
    printf("Loading shader %s finished\n", fileName.c_str());
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

            printf("%s\n%s\n", linkingString.c_str(), acInfoLog);
            delete[] acInfoLog;
        }

        assert(false);
        return 0;
    }

#ifdef RTGI_OUTPUT_RESOURCE_LOADING
    linkingString += " finished";
    printf("%s\n", linkingString.c_str());
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
    GLenum pname, int value)
{
    OpenGLShaderProgramHandle* programHandle =
        (OpenGLShaderProgramHandle*)program->GetProgramHandle();
    glProgramParameteri(programHandle->mProgram, pname, value);

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
    GLint loc = glGetUniformLocation(programHandle->mProgram, name);
    if( !uniform->mUniformHandle )
    {
        uniform->mUniformHandle = new OpenGLShaderUniformHandle();
        uniform->mUniformHandle->Device = this;
    }
    ((OpenGLShaderUniformHandle*)uniform->mUniformHandle)->mUniform = loc;

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueMat4(ShaderUniform* uniform, 
    const mat4* value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
    glUniformMatrix4fv(uniformHandle->mUniform, 1, GL_TRUE, *value);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void OpenGLDevice::__SetUniformValueVec3(ShaderUniform* uniform, 
    const vec3* value)
{
    OpenGLShaderUniformHandle* uniformHandle =
        (OpenGLShaderUniformHandle*)uniform->mUniformHandle;
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
    glUniform2fv(uniformHandle->mUniform, 1, value);

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

    mEnable4xMsaa = false;
    m4xMsaaQuality = 0;
}
//----------------------------------------------------------------------------
OpenGLDevice::~OpenGLDevice()
{
}
//----------------------------------------------------------------------------