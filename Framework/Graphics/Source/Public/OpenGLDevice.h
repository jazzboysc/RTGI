//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_OpenGLDevice_H
#define RTGI_OpenGLDevice_H

// shouldn't include glew.h in header file
#  include <GL/glew.h>
//#include <windows.h>
// #       pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
// #       pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
// #       pragma comment (lib, "gdi32.lib")    /* link Windows GDI lib        */
// #       pragma comment (lib, "winmm.lib")    /* link Windows MultiMedia lib */
// #       pragma comment (lib, "user32.lib")   /* link Windows user lib       */

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
    void __SetProgramParameterInt(ShaderProgram* program, 
        ShaderProgramParameter pname, int value);

    // Uniform.
    void __GetUniformLocation(ShaderProgram* program, ShaderUniform* uniform, 
        const char* name);
    void __SetUniformValueMat4(ShaderUniform* uniform, const glm::mat4* value);
    void __SetUniformValueVec3(ShaderUniform* uniform, const glm::vec3* value);
    void __SetUniformValueInt(ShaderUniform* uniform, int value);
    void __SetUniformValueFloat(ShaderUniform* uniform, float value);
    void __SetUniformValueFloat2(ShaderUniform* uniform, const float* value);

    // Texture.
    void __DeleteTexture(Texture* texture);
    TextureHandle* __Texture1DLoadFromSystemMemory(Texture* texture,
        TextureInternalFormat internalFormat, int width, TextureFormat format, 
        TextureComponentType type, void* pixels);
    void __Texture1DUpdateFromPixelBuffer(Texture* texture, PixelBuffer* pixelBuffer);
    void __TextureBindToImageUnit(Texture* texture, unsigned int unit, BufferAccess access);
    void __Texture1DGetDataFromGPUMemory(Texture* texture, void* dstData);
    TextureHandle* __Texture2DLoadFromSystemMemory(Texture* texture, 
        TextureInternalFormat internalFormat, int width, int height, 
        TextureFormat format, TextureComponentType type, void* pixels);
    TextureHandle* __Texture2DLoadFromTextureBuffer(Texture* texture, 
        TextureBuffer* textureBuffer, TextureInternalFormat internalFormat);
    void __Texture2DUpdateFromPixelBuffer(Texture* texture, PixelBuffer* pixelBuffer);
    void __Texture2DGetImageData(Texture* texture, void* dstPixels);
    TextureHandle* __Tex2DArrayLoadFromSystemMemory(Texture* texture, 
        TextureInternalFormat internalFormat, int width, int height, int depth, 
        TextureFormat format, TextureComponentType type, void* pixels);
    TextureHandle* __Texture3DLoadFromSystemMemory(Texture* texture, 
        TextureInternalFormat internalFormat, int width, int height, int depth, 
        TextureFormat format, TextureComponentType type, void* pixels);
    void __Texture3DUpdateFromPixelBuffer(Texture* texture, PixelBuffer* pixelBuffer);
    TextureHandle* __TextureCubeLoadFromSystemMemory(Texture* texture,
        TextureInternalFormat internalFormat, int width, int height, 
        TextureFormat format, TextureComponentType type,
        void* pixelsPX, void* pixelsNX, void* pixelsPY, void* pixelsNY,
        void* pixelsPZ, void* pixelsNZ);

private:
    bool mEnable4xMsaa;
    UINT m4xMsaaQuality;

};

}

#endif