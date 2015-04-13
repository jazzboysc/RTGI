//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 20inline 3.
//----------------------------------------------------------------------------

#ifndef RTGI_GPUDevice_H
#define RTGI_GPUDevice_H

#include "RefObject.h"
#include "GPUResource.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
struct GPUDeviceDescription
{
    unsigned int FramebufferWidth;
    unsigned int FramebufferHeight;
    bool EnableMSAA;
};

class GPUDevice;
class Shader;
class ShaderProgram;
class PassInfo;
class ShaderUniform;
class Texture;
class Buffer;
class BufferView;
class PixelBuffer;
class TextureBuffer;
class FrameBuffer;

struct ShaderHandle;
struct ShaderProgramHandle;
struct PassInfoHandle;
struct ShaderUniformHandle;
struct TextureHandle;
struct FBOHandle;
struct BufferHandle;
struct BufferViewHandle;
struct SamplerDesc;

enum ShaderProgramParameter;
enum BufferInternalFormat;
enum BufferFormat;
enum BufferComponentType;
enum BufferAccess;
enum BufferUsage;

typedef void (GPUDevice::*GPUDeviceInitialize)(
    GPUDeviceDescription* deviceDesc);
typedef void (GPUDevice::*GPUDeviceTerminate)();
typedef ShaderHandle* (GPUDevice::*GPUDeviceCreateShader)(Shader* shader);
typedef void (GPUDevice::*GPUDeviceDeleteShader)(Shader* shader);
typedef ShaderProgramHandle* (GPUDevice::*GPUDeviceCreateProgram)(
    ShaderProgram* program);
typedef void (GPUDevice::*GPUDeviceDeleteProgram)(ShaderProgram* program);
typedef void (GPUDevice::*GPUDeviceEnableProgram)(ShaderProgram* program);
typedef void (GPUDevice::*GPUDeviceDisableProgram)(ShaderProgram* program);
typedef PassInfoHandle* (GPUDevice::*GPUDeviceCreatePassInfo)(PassInfo* passInfo);
typedef void (GPUDevice::*GPUDeviceDeletePassInfo)(PassInfo* passInfo);
typedef void (GPUDevice::*GPUDeviceGetUniformLocation)(ShaderProgram* program, 
    ShaderUniform* uniform, const char* name);
typedef void (GPUDevice::*GPUDeviceSetUniformValueMat4)(ShaderUniform* uniform, 
    const glm::mat4* value);
typedef void (GPUDevice::*GPUDeviceSetUniformValueVec3)(ShaderUniform* uniform, 
    const glm::vec3* value);
typedef void (GPUDevice::*GPUDeviceSetUniformValueInt)(ShaderUniform* uniform, 
    int value);
typedef void (GPUDevice::*GPUDeviceSetUniformValueUInt)(ShaderUniform* uniform,
    unsigned int value);
typedef void (GPUDevice::*GPUDeviceSetUniformValueFloat)(
    ShaderUniform* uniform, float value);
typedef void (GPUDevice::*GPUDeviceSetUniformValueFloat2)(
    ShaderUniform* uniform, const float* value);
typedef void (GPUDevice::*GPUDeviceSetProgramParameterInt)(
    ShaderProgram* program, ShaderProgramParameter pname, int value);
typedef void (GPUDevice::*GPUDeviceDeleteTexture)(Texture* texture);
typedef TextureHandle* (GPUDevice::*GPUDeviceTexture1DLoadFromSystemMemory)(
    Texture* texture, BufferInternalFormat internalFormat, int width, 
    BufferFormat format, BufferComponentType type, void* pixels);
typedef void (GPUDevice::*GPUDeviceTexture1DUpdateFromPixelBuffer)(
    Texture* texture, PixelBuffer* pixelBuffer);
typedef void (GPUDevice::*GPUDeviceTextureBindToImageUnit)(Texture* texture, 
	unsigned int unit, bool layered, BufferAccess access);
typedef void (GPUDevice::*GPUDeviceTextureBindToSampler)(Texture* texture, 
    unsigned int index, SamplerDesc* sampler);
typedef void (GPUDevice::*GPUDeviceTextureGenerateMipmap)(Texture* texture);
typedef void (GPUDevice::*GPUDeviceTexture1DGetDataFromGPUMemory)(
    Texture* texture, void* dstData);
typedef TextureHandle* (GPUDevice::*GPUDeviceTexture2DLoadFromSystemMemory)(
    Texture* texture, BufferInternalFormat internalFormat, int width, 
    int height, BufferFormat format, BufferComponentType type, bool mipMap, 
    SamplerDesc* sampler, void* pixels);
typedef TextureHandle* (GPUDevice::*GPUDeviceTexture2DLoadFromTextureBuffer)(
    Texture* texture, TextureBuffer* textureBuffer, 
    BufferInternalFormat internalFormat);
typedef void (GPUDevice::*GPUDeviceTexture2DUpdateFromPixelBuffer)(
    Texture* texture, PixelBuffer* pixelBuffer);
typedef void (GPUDevice::*GPUDeviceTexture2DGetImageData)(Texture* texture, 
    void* dstPixels);
typedef TextureHandle* (GPUDevice::*GPUDeviceTex2DArrayLoadFromSystemMemory)(
    Texture* texture, BufferInternalFormat internalFormat, int width,
    int height, int depth, BufferFormat format, BufferComponentType type,
	bool mipMap, void* pixels);
typedef TextureHandle* (GPUDevice::*GPUDeviceTexture3DLoadFromSystemMemory)(
    Texture* texture, BufferInternalFormat internalFormat, int width, 
    int height, int depth, BufferFormat format, BufferComponentType type, 
    void* pixels);
typedef void (GPUDevice::*GPUDeviceTexture3DUpdateFromPixelBuffer)(
    Texture* texture, PixelBuffer* pixelBuffer);
typedef TextureHandle* (GPUDevice::*GPUDeviceTextureCubeLoadFromSystemMemory)(
    Texture* texture, BufferInternalFormat internalFormat, int width,
    int height, BufferFormat format, BufferComponentType type, bool mipMap,
    void* pixelsPX, void* pixelsNX, void* pixelsPY, void* pixelsNY, 
    void* pixelsPZ, void* pixelsNZ);
typedef TextureHandle* (GPUDevice::*GPUDeviceBufferTextureLoadFromTextureBuffer)(
    Texture* texture, TextureBuffer* textureBuffer, 
    BufferInternalFormat internalFormat);
typedef FBOHandle* (GPUDevice::*GPUDeviceCreateFrameBuffer)(
    FrameBuffer* frameBuffer);
typedef void (GPUDevice::*GPUDeviceDeleteFrameBuffer)(
    FrameBuffer* frameBuffer);
typedef void (GPUDevice::*GPUDeviceFrameBufferSetRenderTargets)(
    FrameBuffer* frameBuffer, unsigned int colorTextureCount, 
    Texture** colorTextures, Texture* depthTexture, Texture* stencilTexture);
typedef void (GPUDevice::*GPUDeviceFrameBufferSetColorTarget)(FrameBuffer* frameBuffer,
    unsigned int attachPoint, Texture* colorTexture);
typedef void (GPUDevice::*GPUDeviceFrameBufferSetDepthTarget)(FrameBuffer* frameBuffer, 
    Texture* depthTexture);
typedef void (GPUDevice::*GPUDeviceFrameBufferSetStencilTarget)(FrameBuffer* frameBuffer, 
    Texture* stencilTexture);
typedef void (GPUDevice::*GPUDeviceFrameBufferEnable)(
    FrameBuffer* frameBuffer);
typedef void (GPUDevice::*GPUDeviceFrameBufferDisable)(
    FrameBuffer* frameBuffer);
typedef void (GPUDevice::*GPUDeviceComputeShaderDispatch)(
    ShaderProgram* program, unsigned int globalX, unsigned int globalY, 
    unsigned int globalZ);
typedef void (GPUDevice::*GPUDeviceDispatchVertex)(unsigned int threadCount);
typedef void (GPUDevice::*GPUDeviceDispatchVertexIndirect)(void* indirect);
typedef void (GPUDevice::*GPUDeviceDeleteBuffer)(Buffer* buffer);
typedef void* (GPUDevice::*GPUDeviceBufferMap)(Buffer* buffer,
    BufferAccess access);
typedef void (GPUDevice::*GPUDeviceBufferUnmap)(Buffer* buffer);
typedef void (GPUDevice::*GPUDeviceBufferBindIndex)(Buffer* buffer, 
    unsigned int index);
typedef void (GPUDevice::*GPUDeviceBufferBindIndexTo)(Buffer* buffer,
    unsigned int index, BufferView* view);
typedef void (GPUDevice::*GPUDeviceBufferBind)(Buffer* buffer);
typedef void (GPUDevice::*GPUDeviceBufferBindTo)(Buffer* buffer, BufferView* view);
typedef BufferViewHandle* (GPUDevice::*GPUDeviceCreateBufferView)(BufferView* view);
typedef void (GPUDevice::*GPUDeviceDeleteBufferView)(BufferView* view);
typedef void (GPUDevice::*GPUDeviceBufferUpdateSubData)(Buffer* buffer, 
    int offset, size_t size, void* data);
typedef BufferHandle* (GPUDevice::*GPUDeviceBufferLoadFromSystemMemory)(
    Buffer* buffer, size_t size, void* data, BufferUsage usage);
typedef BufferHandle* 
    (GPUDevice::*GPUDeviceBufferLoadImmutableFromSystemMemory)(Buffer* buffer, 
    size_t size, void* data);
typedef void (GPUDevice::*GPUDeviceBufferClear)(Buffer* buffer, 
    BufferInternalFormat internalFormat, BufferFormat format, 
    BufferComponentType type, void* data);
typedef void (GPUDevice::*GPUDeviceGetMaxAnisFilterLevel)(int* maxAnisFilterLevel);
typedef void (GPUDevice::*GPUDeviceSetAnisFilterLevel)(int maxAnisFilterLevel);

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class GPUDevice : public RefObject
{
public:
    GPUDevice();
    ~GPUDevice();
	
    inline 	void Initialize(GPUDeviceDescription* deviceDesc);
    inline 	void Terminate();
    inline 	ShaderHandle* CreateShader(Shader* shader);
    inline 	void DeleteShader(Shader* shader);
    inline 	ShaderProgramHandle* CreateProgram(ShaderProgram* program);
 
    inline 	void DeleteProgram(ShaderProgram* program);
    inline 	void EnableProgram(ShaderProgram* program);
    inline 	void DisableProgram(ShaderProgram* program);
    inline 	PassInfoHandle* CreatePassInfo(PassInfo* passInfo);
    inline 	void DeletePassInfo(PassInfo* passInfo);
    inline 	void GetUniformLocation(ShaderProgram* program, 
        ShaderUniform* uniform, const char* name);
    inline 	void SetUniformValueMat4(ShaderUniform* uniform,
        const glm::mat4* value);
    inline 	void SetUniformValueVec3(ShaderUniform* uniform,
        const glm::vec3* value);
    inline 	void SetUniformValueInt(ShaderUniform* uniform,
        int value);
    inline 	void SetUniformValueUInt(ShaderUniform* uniform,
        unsigned int value);
    inline 	void SetUniformValueFloat(ShaderUniform* uniform, float value);
    inline 	void SetUniformValueFloat2(ShaderUniform* uniform, 
        const float* value);
    inline 	void SetProgramParameterInt(ShaderProgram* program, 
        ShaderProgramParameter pname, int value);
    inline 	void DeleteTexture(Texture* texture);
    inline TextureHandle* Texture1DLoadFromSystemMemory(Texture* texture, 
        BufferInternalFormat internalFormat, int width, BufferFormat format, 
        BufferComponentType type, void* pixels);
    inline 	void Texture1DUpdateFromPixelBuffer(Texture* texture, 
        PixelBuffer* pixelBuffer);
    inline 	void TextureBindToImageUnit(Texture* texture, unsigned int unit,
		bool layered, BufferAccess access);
    inline 	void TextureBindToSampler(Texture* texture, unsigned int index, 
        SamplerDesc* sampler);
    inline void TextureGenerateMipmap(Texture* texture);
    inline  void Texture1DGetDataFromGPUMemory(Texture* texture, 
        void* dstData);
    inline 	TextureHandle* Texture2DLoadFromSystemMemory(Texture* texture, 
        BufferInternalFormat internalFormat, int width, int height, 
        BufferFormat format, BufferComponentType type, bool mipMap, 
        SamplerDesc* sampler, void* pixels);
    inline 	TextureHandle* Texture2DLoadFromTextureBuffer(Texture* texture, 
        TextureBuffer* textureBuffer, BufferInternalFormat internalFormat);
    inline 	void Texture2DUpdateFromPixelBuffer(Texture* texture, 
        PixelBuffer* pixelBuffer);
    inline 	void Texture2DGetImageData(Texture* texture, void* dstPixels);
    inline 	TextureHandle* Tex2DArrayLoadFromSystemMemory(Texture* texture, 
        BufferInternalFormat internalFormat, int width, int height, 
        int depth, BufferFormat format, BufferComponentType type, 
		bool mipMap, void* pixels);
    inline 	TextureHandle* Texture3DLoadFromSystemMemory(Texture* texture, 
        BufferInternalFormat internalFormat, int width, int height, 
        int depth, BufferFormat format, BufferComponentType type, 
        void* pixels);
    inline 	void Texture3DUpdateFromPixelBuffer(Texture* texture, 
        PixelBuffer* pixelBuffer);
    inline 	TextureHandle* TextureCubeLoadFromSystemMemory(Texture* texture, 
        BufferInternalFormat internalFormat, int width, int height, 
        BufferFormat format, BufferComponentType type, bool mipMap,
        void* pixelsPX, void* pixelsNX, void* pixelsPY, void* pixelsNY,
        void* pixelsPZ, void* pixelsNZ);
    inline  TextureHandle* BufferTextureLoadFromTextureBuffer(
        Texture* texture, TextureBuffer* textureBuffer,
        BufferInternalFormat internalFormat);
    inline 	FBOHandle* CreateFrameBuffer(FrameBuffer* frameBuffer);
    inline 	void DeleteFrameBuffer(FrameBuffer* frameBuffer);
    inline 	void FrameBufferSetRenderTargets(FrameBuffer* frameBuffer, 
        unsigned int colorTextureCount, Texture** colorTextures, 
        Texture* depthTexture, Texture* stencilTexture);
    inline  void FrameBufferSetColorTarget(FrameBuffer* frameBuffer, 
        unsigned int attachPoint, Texture* colorTexture);
    inline  void FrameBufferSetDepthTarget(FrameBuffer* frameBuffer, 
        Texture* depthTexture);
    inline  void FrameBufferSetStencilTarget(FrameBuffer* frameBuffer, 
        Texture* stencilTexture);
    inline 	void FrameBufferEnable(FrameBuffer* frameBuffer);
    inline 	void FrameBufferDisable(FrameBuffer* frameBuffer);
    inline 	void ComputeShaderDispatch(ShaderProgram* program, 
        unsigned int globalX, unsigned int globalY, unsigned int globalZ);
    inline 	void DispatchVertex(unsigned int threadCount);
    inline 	void DispatchVertexIndirect(void* indirect);
    inline 	void DeleteBuffer(Buffer* buffer);
    inline 	void* BufferMap(Buffer* buffer, BufferAccess access);
    inline 	void BufferUnmap(Buffer* buffer);
    inline 	void BufferBindIndex(Buffer* buffer, unsigned int index);
    inline  void BufferBindIndexTo(Buffer* buffer, unsigned int index, BufferView* view);
    inline 	void BufferBind(Buffer* buffer);
    inline  void BufferBindTo(Buffer* buffer, BufferView* view);
    inline  BufferViewHandle* CreateBufferView(BufferView* view);
    inline  void DeleteBufferView(BufferView* view);
    inline 	void BufferUpdateSubData(Buffer* buffer, int offset, size_t size, 
        void* data);
    inline 	BufferHandle* BufferLoadFromSystemMemory(Buffer* buffer, 
        size_t size, void* data, BufferUsage usage);
    inline 	BufferHandle* BufferLoadImmutableFromSystemMemory(Buffer* buffer,
        size_t size, void* data);
    inline 	void BufferClear(Buffer* buffer, 
        BufferInternalFormat internalFormat, BufferFormat format,
        BufferComponentType type, void* data);
    inline 	void GetMaxAnisFilterLevel(int* maxAnisFilterLevel);
    inline 	void SetAnisFilterLevel(int maxAnisFilterLevel);

protected:
    GPUDeviceInitialize                           _Initialize;
    GPUDeviceTerminate                            _Terminate;
    GPUDeviceCreateShader                         _CreateShader;
    GPUDeviceDeleteShader                         _DeleteShader;
    GPUDeviceCreateProgram                        _CreateProgram;
    GPUDeviceDeleteProgram                        _DeleteProgram;
    GPUDeviceEnableProgram                        _EnableProgram;
    GPUDeviceDisableProgram                       _DisableProgram;
    GPUDeviceCreatePassInfo                       _CreatePassInfo;
    GPUDeviceDeletePassInfo                       _DeletePassInfo;
    GPUDeviceGetUniformLocation                   _GetUniformLocation;
    GPUDeviceSetUniformValueMat4                  _SetUniformValueMat4;
    GPUDeviceSetUniformValueVec3                  _SetUniformValueVec3;
    GPUDeviceSetUniformValueInt                   _SetUniformValueInt;
    GPUDeviceSetUniformValueUInt                  _SetUniformValueUInt;
    GPUDeviceSetUniformValueFloat                 _SetUniformValueFloat;
    GPUDeviceSetUniformValueFloat2                _SetUniformValueFloat2;
    GPUDeviceSetProgramParameterInt               _SetProgramParameterInt;
    GPUDeviceDeleteTexture                        _DeleteTexture;
    GPUDeviceTexture1DLoadFromSystemMemory        _Texture1DLoadFromSystemMemory;
    GPUDeviceTexture1DUpdateFromPixelBuffer       _Texture1DUpdateFromPixelBuffer;
    GPUDeviceTextureBindToImageUnit               _TextureBindToImageUnit;
    GPUDeviceTextureBindToSampler                 _TextureBindToSampler;
    GPUDeviceTextureGenerateMipmap                _TextureGenerateMipmap;
    GPUDeviceTexture1DGetDataFromGPUMemory        _Texture1DGetDataFromGPUMemory;
    GPUDeviceTexture2DLoadFromSystemMemory        _Texture2DLoadFromSystemMemory;
    GPUDeviceTexture2DLoadFromTextureBuffer       _Texture2DLoadFromTextureBuffer;
    GPUDeviceTexture2DUpdateFromPixelBuffer       _Texture2DUpdateFromPixelBuffer;
    GPUDeviceTexture2DGetImageData                _Texture2DGetImageData;
    GPUDeviceTex2DArrayLoadFromSystemMemory       _Tex2DArrayLoadFromSystemMemory;
    GPUDeviceTexture3DLoadFromSystemMemory        _Texture3DLoadFromSystemMemory;
    GPUDeviceTexture3DUpdateFromPixelBuffer       _Texture3DUpdateFromPixelBuffer;
    GPUDeviceTextureCubeLoadFromSystemMemory      _TextureCubeLoadFromSystemMemory;
    GPUDeviceBufferTextureLoadFromTextureBuffer   _BufferTextureLoadFromTextureBuffer;
    GPUDeviceCreateFrameBuffer                    _CreateFrameBuffer;
    GPUDeviceDeleteFrameBuffer                    _DeleteFrameBuffer;
    GPUDeviceFrameBufferSetRenderTargets          _FrameBufferSetRenderTargets;
    GPUDeviceFrameBufferSetColorTarget            _FrameBufferSetColorTarget;
    GPUDeviceFrameBufferSetDepthTarget            _FrameBufferSetDepthTarget;
    GPUDeviceFrameBufferSetStencilTarget          _FrameBufferSetStencilTarget;
    GPUDeviceFrameBufferEnable                    _FrameBufferEnable;
    GPUDeviceFrameBufferDisable                   _FrameBufferDisable;
    GPUDeviceComputeShaderDispatch                _ComputeShaderDispatch;
    GPUDeviceDispatchVertex                       _DispatchVertex;
    GPUDeviceDispatchVertexIndirect               _DispatchVertexIndirect;
    GPUDeviceDeleteBuffer                         _DeleteBuffer;
    GPUDeviceBufferMap                            _BufferMap;
    GPUDeviceBufferUnmap                          _BufferUnmap;
    GPUDeviceBufferBindIndex                      _BufferBindIndex;
    GPUDeviceBufferBindIndexTo                    _BufferBindIndexTo;
    GPUDeviceBufferBind                           _BufferBind;
    GPUDeviceBufferBindTo                         _BufferBindTo;
    GPUDeviceCreateBufferView                     _CreateBufferView;
    GPUDeviceDeleteBufferView                     _DeleteBufferView;
    GPUDeviceBufferUpdateSubData                  _BufferUpdateSubData;
    GPUDeviceBufferLoadFromSystemMemory           _BufferLoadFromSystemMemory;
    GPUDeviceBufferLoadImmutableFromSystemMemory  _BufferLoadImmutableFromSystemMemory;
    GPUDeviceBufferClear                          _BufferClear;
	GPUDeviceGetMaxAnisFilterLevel				  _GetMaxAnisFilterLevel;
	GPUDeviceSetAnisFilterLevel					  _SetAnisFilterLevel;

};

typedef RefPointer<GPUDevice> GPUDevicePtr;

#include "GPUDevice.inl"

}

#endif