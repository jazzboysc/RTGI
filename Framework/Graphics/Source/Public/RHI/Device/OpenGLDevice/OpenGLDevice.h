//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_OpenGLDevice_H
#define RTGI_OpenGLDevice_H

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

	// Misc.
	int __IsExtensionSupported(const char *extension);
	void __GetMaxAnisFilterLevel(int* maxAnisFilterLevel);
	void __SetAnisFilterLevel(int anisFilterLevel);

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
	void __SetUniformValueVec4(ShaderUniform* uniform, const glm::vec4* value);
    void __SetUniformValueInt(ShaderUniform* uniform, int value);
    void __SetUniformValueUInt(ShaderUniform* uniform, unsigned int value);
    void __SetUniformValueFloat(ShaderUniform* uniform, float value);
    void __SetUniformValueFloat2(ShaderUniform* uniform, const float* value);

    // Texture.
    void __DeleteTexture(Texture* texture);
    TextureHandle* __Texture1DLoadFromSystemMemory(Texture* texture,
        BufferInternalFormat internalFormat, int width, BufferFormat format, 
        BufferComponentType type, void* pixels);
    void __Texture1DUpdateFromPixelBuffer(Texture* texture, PixelBuffer* pixelBuffer);
	void __TextureBindToImageUnit(Texture* texture, unsigned int unit, bool layered, BufferAccess access);
    void __TextureBindToSampler(Texture* texture, unsigned int index, SamplerDesc* sampler);
    void __TextureGenerateMipmap(Texture* texture);
    void __Texture1DGetDataFromGPUMemory(Texture* texture, void* dstData);
    TextureHandle* __Texture2DLoadFromSystemMemory(Texture* texture, 
        BufferInternalFormat internalFormat, int width, int height, 
        BufferFormat format, BufferComponentType type, bool mipMap, 
        SamplerDesc* sampler, void* pixels);
    TextureHandle* __Texture2DLoadFromTextureBuffer(Texture* texture, 
        TextureBuffer* textureBuffer, BufferInternalFormat internalFormat);
    void __Texture2DUpdateFromPixelBuffer(Texture* texture, PixelBuffer* pixelBuffer);
    void __Texture2DGetImageData(Texture* texture, void* dstPixels);
    TextureHandle* __Tex2DArrayLoadFromSystemMemory(Texture* texture, 
        BufferInternalFormat internalFormat, int width, int height, int depth, 
		BufferFormat format, BufferComponentType type, bool mipMap, void* pixels);
    TextureHandle* __Texture3DLoadFromSystemMemory(Texture* texture, 
        BufferInternalFormat internalFormat, int width, int height, int depth, 
        BufferFormat format, BufferComponentType type, void* pixels);
    void __Texture3DUpdateFromPixelBuffer(Texture* texture, PixelBuffer* pixelBuffer);
    TextureHandle* __TextureCubeLoadFromSystemMemory(Texture* texture,
        BufferInternalFormat internalFormat, int width, int height, 
        BufferFormat format, BufferComponentType type, bool mipMap, 
        void* pixelsPX, void* pixelsNX, void* pixelsPY, void* pixelsNY,
        void* pixelsPZ, void* pixelsNZ);
    TextureHandle* __BufferTextureLoadFromTextureBuffer(Texture* texture, 
        TextureBuffer* textureBuffer, BufferInternalFormat internalFormat);

    // FrameBuffer.
    FBOHandle* __CreateFrameBuffer(FrameBuffer* frameBuffer);
    void __DeleteFrameBuffer(FrameBuffer* frameBuffer);
    void __FrameBufferSetRenderTargets(FrameBuffer* frameBuffer, 
        unsigned int colorTextureCount, Texture** colorTextures, 
        Texture* depthTexture, Texture* stencilTexture);
    void __FrameBufferSetColorTarget(FrameBuffer* frameBuffer, 
        unsigned int attachPoint, Texture* colorTexture);
    void __FrameBufferSetDepthTarget(FrameBuffer* frameBuffer, 
        Texture* depthTexture);
    void __FrameBufferSetStencilTarget(FrameBuffer* frameBuffer, 
        Texture* stencilTexture);
    void __FrameBufferEnable(FrameBuffer* frameBuffer);
    void __FrameBufferDisable(FrameBuffer* frameBuffer);

    // Compute.
    void __ComputeShaderDispatch(ShaderProgram* program, unsigned int globalX, 
        unsigned int globalY, unsigned int globalZ);
	void __ComputeShaderDispatchIndirect(void* indirect);
	void __DispatchVertex(unsigned int threadCount);
    void __DispatchVertexIndirect(void* indirect);

    // Buffer.
    void __DeleteBuffer(Buffer* buffer);
    void* __BufferMap(Buffer* buffer, BufferAccess access);
    void __BufferUnmap(Buffer* buffer);
    void __BufferBindIndex(Buffer* buffer, unsigned int index);
    void __BufferBindIndexTo(Buffer* buffer, unsigned int index, BufferView* view);
    void __BufferBind(Buffer* buffer);
    void __BufferBindTo(Buffer* buffer, BufferView* view);
    BufferViewHandle* __CreateBufferView(BufferView* view);
    void __DeleteBufferView(BufferView* view);
    void __BufferUpdateSubData(Buffer* buffer, int offset, size_t size, void* data);
    BufferHandle* __BufferLoadFromSystemMemory(Buffer* buffer, size_t size, 
        void* data, BufferUsage usage);
    BufferHandle* __BufferLoadImmutableFromSystemMemory(Buffer* buffer, 
        size_t size, void* data);
    void __BufferClear(Buffer* buffer, BufferInternalFormat internalFormat, 
        BufferFormat format, BufferComponentType type, void* data);

private:
    bool mEnable4xMsaa;
    unsigned int m4xMsaaQuality;
	GLint mAnisFilterLevel;
};

}

#endif