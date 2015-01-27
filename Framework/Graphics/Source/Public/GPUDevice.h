//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_GPUDevice_H
#define RTGI_GPUDevice_H

#include "RefObject.h"
#include "GPUResource.h"

namespace RTGI
{

#define GPU_DEVICE_FUNC(device, function) (device->*device->function)
#define GPU_DEVICE_FUNC_SetUniformValueMat4(uniform, value) \
    GPU_DEVICE_FUNC((uniform).mUniformHandle->Device, SetUniformValueMat4) \
    (&(uniform), &(value))
#define GPU_DEVICE_FUNC_SetUniformValueVec3(uniform, value) \
    GPU_DEVICE_FUNC((uniform).mUniformHandle->Device, SetUniformValueVec3) \
    (&(uniform), &(value))
#define GPU_DEVICE_FUNC_SetUniformValueInt(uniform, value) \
    GPU_DEVICE_FUNC((uniform).mUniformHandle->Device, SetUniformValueInt) \
    (&(uniform), value)
#define GPU_DEVICE_FUNC_SetUniformValueFloat(uniform, value) \
    GPU_DEVICE_FUNC((uniform).mUniformHandle->Device, SetUniformValueFloat) \
    (&(uniform), value)
#define GPU_DEVICE_FUNC_SetUniformValueFloat2(uniform, value) \
    GPU_DEVICE_FUNC((uniform).mUniformHandle->Device, SetUniformValueFloat2) \
    (&(uniform), value)
#define GPU_DEVICE_FUNC_GetUniformLocation(program, uniform, name) \
    GPU_DEVICE_FUNC((program)->GetProgramHandle()->Device, GetUniformLocation) \
    (program, &(uniform), name)
#define GPU_DEVICE_FUNC_SetProgramParameterInt(program, pname, value) \
    GPU_DEVICE_FUNC((program)->GetProgramHandle()->Device, \
    SetProgramParameterInt)(program, pname, value)

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/13/2014
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
    unsigned int unit, BufferAccess access);
typedef void (GPUDevice::*GPUDeviceTextureBindToSampler)(Texture* texture, 
    unsigned int index, SamplerDesc* sampler);
typedef void (GPUDevice::*GPUDeviceTexture1DGetDataFromGPUMemory)(
    Texture* texture, void* dstData);
typedef TextureHandle* (GPUDevice::*GPUDeviceTexture2DLoadFromSystemMemory)(
    Texture* texture, BufferInternalFormat internalFormat, int width, 
    int height, BufferFormat format, BufferComponentType type, bool mipMap,
    void* pixels);
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
    void* pixels);
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
typedef FBOHandle* (GPUDevice::*GPUDeviceCreateFrameBuffer)(
    FrameBuffer* frameBuffer);
typedef void (GPUDevice::*GPUDeviceDeleteFrameBuffer)(
    FrameBuffer* frameBuffer);
typedef void (GPUDevice::*GPUDeviceFrameBufferSetRenderTargets)(
    FrameBuffer* frameBuffer, unsigned int colorTextureCount, 
    Texture** colorTextures, Texture* depthTexture, Texture* stencilTexture);
typedef void (GPUDevice::*GPUDeviceFrameBufferEnable)(
    FrameBuffer* frameBuffer);
typedef void (GPUDevice::*GPUDeviceFrameBufferDisable)(
    FrameBuffer* frameBuffer);
typedef void (GPUDevice::*GPUDeviceComputeShaderDispatch)(
    ShaderProgram* program, unsigned int globalX, unsigned int globalY, 
    unsigned int globalZ);
typedef void (GPUDevice::*GPUDeviceDispatchVertexIndirect)(void* indirect);
typedef void (GPUDevice::*GPUDeviceDeleteBuffer)(Buffer* buffer);
typedef void* (GPUDevice::*GPUDeviceBufferMap)(Buffer* buffer,
    BufferAccess access);
typedef void (GPUDevice::*GPUDeviceBufferUnmap)(Buffer* buffer);
typedef void (GPUDevice::*GPUDeviceBufferBindIndex)(Buffer* buffer, 
    unsigned int index);
typedef void (GPUDevice::*GPUDeviceBufferBind)(Buffer* buffer);
typedef void (GPUDevice::*GPUDeviceBufferBindToIndirect)(Buffer* buffer);
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

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/13/2014
//----------------------------------------------------------------------------
class GPUDevice : public RefObject
{
public:
    GPUDevice();
    ~GPUDevice();

    GPUDeviceInitialize                           Initialize;
    GPUDeviceTerminate                            Terminate;
    GPUDeviceCreateShader                         CreateShader;
    GPUDeviceDeleteShader                         DeleteShader;
    GPUDeviceCreateProgram                        CreateProgram;
    GPUDeviceDeleteProgram                        DeleteProgram;
    GPUDeviceEnableProgram                        EnableProgram;
    GPUDeviceDisableProgram                       DisableProgram;
    GPUDeviceCreatePassInfo                       CreatePassInfo;
    GPUDeviceDeletePassInfo                       DeletePassInfo;
    GPUDeviceGetUniformLocation                   GetUniformLocation;
    GPUDeviceSetUniformValueMat4                  SetUniformValueMat4;
    GPUDeviceSetUniformValueVec3                  SetUniformValueVec3;
    GPUDeviceSetUniformValueInt                   SetUniformValueInt;
    GPUDeviceSetUniformValueFloat                 SetUniformValueFloat;
    GPUDeviceSetUniformValueFloat2                SetUniformValueFloat2;
    GPUDeviceSetProgramParameterInt               SetProgramParameterInt;
    GPUDeviceDeleteTexture                        DeleteTexture;
    GPUDeviceTexture1DLoadFromSystemMemory        Texture1DLoadFromSystemMemory;
    GPUDeviceTexture1DUpdateFromPixelBuffer       Texture1DUpdateFromPixelBuffer;
    GPUDeviceTextureBindToImageUnit               TextureBindToImageUnit;
    GPUDeviceTextureBindToSampler                 TextureBindToSampler;
    GPUDeviceTexture1DGetDataFromGPUMemory        Texture1DGetDataFromGPUMemory;
    GPUDeviceTexture2DLoadFromSystemMemory        Texture2DLoadFromSystemMemory;
    GPUDeviceTexture2DLoadFromTextureBuffer       Texture2DLoadFromTextureBuffer;
    GPUDeviceTexture2DUpdateFromPixelBuffer       Texture2DUpdateFromPixelBuffer;
    GPUDeviceTexture2DGetImageData                Texture2DGetImageData;
    GPUDeviceTex2DArrayLoadFromSystemMemory       Tex2DArrayLoadFromSystemMemory;
    GPUDeviceTexture3DLoadFromSystemMemory        Texture3DLoadFromSystemMemory;
    GPUDeviceTexture3DUpdateFromPixelBuffer       Texture3DUpdateFromPixelBuffer;
    GPUDeviceTextureCubeLoadFromSystemMemory      TextureCubeLoadFromSystemMemory;
    GPUDeviceCreateFrameBuffer                    CreateFrameBuffer;
    GPUDeviceDeleteFrameBuffer                    DeleteFrameBuffer;
    GPUDeviceFrameBufferSetRenderTargets          FrameBufferSetRenderTargets;
    GPUDeviceFrameBufferEnable                    FrameBufferEnable;
    GPUDeviceFrameBufferDisable                   FrameBufferDisable;
    GPUDeviceComputeShaderDispatch                ComputeShaderDispatch;
    GPUDeviceDispatchVertexIndirect               DispatchVertexIndirect;
    GPUDeviceDeleteBuffer                         DeleteBuffer;
    GPUDeviceBufferMap                            BufferMap;
    GPUDeviceBufferUnmap                          BufferUnmap;
    GPUDeviceBufferBindIndex                      BufferBindIndex;
    GPUDeviceBufferBind                           BufferBind;
    GPUDeviceBufferBindToIndirect                 BufferBindToIndirect;
    GPUDeviceBufferUpdateSubData                  BufferUpdateSubData;
    GPUDeviceBufferLoadFromSystemMemory           BufferLoadFromSystemMemory;
    GPUDeviceBufferLoadImmutableFromSystemMemory  BufferLoadImmutableFromSystemMemory;
    GPUDeviceBufferClear                          BufferClear;
};

typedef RefPointer<GPUDevice> GPUDevicePtr;

}

#endif