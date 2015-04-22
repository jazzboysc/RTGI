//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void GPUDevice::BufferClear(Buffer* buffer, 
    BufferInternalFormat internalFormat, BufferFormat format, 
    BufferComponentType type, void* data)
{
    (this->*_BufferClear)(buffer, internalFormat, format, type, data);
}
//----------------------------------------------------------------------------
BufferHandle* GPUDevice::BufferLoadImmutableFromSystemMemory(Buffer* buffer, 
    size_t size, void* data)
{
    return (this->*_BufferLoadImmutableFromSystemMemory)(buffer, size, data);
}
//----------------------------------------------------------------------------
BufferHandle* GPUDevice::BufferLoadFromSystemMemory(Buffer* buffer, 
    size_t size, void* data, BufferUsage usage)
{
    return (this->*_BufferLoadFromSystemMemory)(buffer, size, data, usage);
}
//----------------------------------------------------------------------------
void GPUDevice::BufferUpdateSubData(Buffer* buffer, int offset, size_t size, 
    void* data)
{
    (this->*_BufferUpdateSubData)(buffer, offset, size, data);
}
//----------------------------------------------------------------------------
void GPUDevice::BufferBind(Buffer* buffer)
{
    (this->*_BufferBind)(buffer);
}
//----------------------------------------------------------------------------
void GPUDevice::BufferBindTo(Buffer* buffer, BufferView* view)
{
    (this->*_BufferBindTo)(buffer, view);
}
//----------------------------------------------------------------------------
BufferViewHandle* GPUDevice::CreateBufferView(BufferView* view)
{
    return (this->*_CreateBufferView)(view);
}
//----------------------------------------------------------------------------
void GPUDevice::DeleteBufferView(BufferView* view)
{
    (this->*_DeleteBufferView)(view);
}
//----------------------------------------------------------------------------
void GPUDevice::BufferBindIndex(Buffer* buffer, unsigned int index)
{
    (this->*_BufferBindIndex)(buffer, index);
}
//----------------------------------------------------------------------------
void GPUDevice::BufferBindIndexTo(Buffer* buffer, unsigned int index, 
    BufferView* view)
{
    (this->*_BufferBindIndexTo)(buffer, index, view);
}
//----------------------------------------------------------------------------
void GPUDevice::BufferUnmap(Buffer* buffer)
{
    (this->*_BufferUnmap)(buffer);
}
//----------------------------------------------------------------------------
void* GPUDevice::BufferMap(Buffer* buffer, BufferAccess access)
{
    return (this->*_BufferMap)(buffer, access);
}
//----------------------------------------------------------------------------
void GPUDevice::DeleteBuffer(Buffer* buffer)
{
    (this->*_DeleteBuffer)(buffer);
}
//----------------------------------------------------------------------------
void GPUDevice::ComputeShaderDispatch(ShaderProgram* program,
	unsigned int globalX, unsigned int globalY, unsigned int globalZ)
{
	(this->*_ComputeShaderDispatch)(program, globalX, globalY, globalZ);
}
//----------------------------------------------------------------------------
void GPUDevice::ComputeShaderDispatchIndirect(void* indirect)
{
	(this->*_ComputeShaderDispatchIndirect)(indirect);
}
//----------------------------------------------------------------------------
void GPUDevice::DispatchVertexIndirect(void* indirect)
{
    (this->*_DispatchVertexIndirect)(indirect);
}
//----------------------------------------------------------------------------
void GPUDevice::DispatchVertex(unsigned int threadCount)
{
    (this->*_DispatchVertex)(threadCount);
}
//----------------------------------------------------------------------------
void GPUDevice::FrameBufferDisable(FrameBuffer* frameBuffer)
{
    (this->*_FrameBufferDisable)(frameBuffer);
}
//----------------------------------------------------------------------------
void GPUDevice::FrameBufferEnable(FrameBuffer* frameBuffer)
{
    (this->*_FrameBufferEnable)(frameBuffer);
}
//----------------------------------------------------------------------------
void GPUDevice::FrameBufferSetRenderTargets(FrameBuffer* frameBuffer,
    unsigned int colorTextureCount, Texture** colorTextures,
    Texture* depthTexture, Texture* stencilTexture)
{
    (this->*_FrameBufferSetRenderTargets)(frameBuffer, colorTextureCount,
        colorTextures, depthTexture, stencilTexture);
}
//----------------------------------------------------------------------------
void GPUDevice::FrameBufferSetColorTarget(FrameBuffer* frameBuffer, 
    unsigned int attachPoint, Texture* colorTexture)
{
    (this->*_FrameBufferSetColorTarget)(frameBuffer, attachPoint, 
        colorTexture);
}
//----------------------------------------------------------------------------
void GPUDevice::FrameBufferSetDepthTarget(FrameBuffer* frameBuffer, 
    Texture* depthTexture)
{
    (this->*_FrameBufferSetDepthTarget)(frameBuffer, depthTexture);
}
//----------------------------------------------------------------------------
void GPUDevice::FrameBufferSetStencilTarget(FrameBuffer* frameBuffer, 
    Texture* stencilTexture)
{
    (this->*_FrameBufferSetStencilTarget)(frameBuffer, stencilTexture);
}
//----------------------------------------------------------------------------
void GPUDevice::DeleteFrameBuffer(FrameBuffer* frameBuffer)
{
    (this->*_DeleteFrameBuffer)(frameBuffer);
}
//----------------------------------------------------------------------------
FBOHandle* GPUDevice::CreateFrameBuffer(FrameBuffer* frameBuffer)
{
    return (this->*_CreateFrameBuffer)(frameBuffer);
}
//----------------------------------------------------------------------------
TextureHandle* GPUDevice::TextureCubeLoadFromSystemMemory(Texture* texture,
    BufferInternalFormat internalFormat, int width, int height,
    BufferFormat format, BufferComponentType type, bool mipMap,
    void* pixelsPX, void* pixelsNX, void* pixelsPY, void* pixelsNY,
    void* pixelsPZ, void* pixelsNZ)
{
    return (this->*_TextureCubeLoadFromSystemMemory)(texture, internalFormat, 
        width, height, format, type, mipMap, pixelsPX, pixelsNX, pixelsPY, 
        pixelsNY, pixelsPZ, pixelsNZ);
}
//----------------------------------------------------------------------------
TextureHandle* GPUDevice::BufferTextureLoadFromTextureBuffer(Texture* texture,
    TextureBuffer* textureBuffer, BufferInternalFormat internalFormat)
{
    return (this->*_BufferTextureLoadFromTextureBuffer)(texture, 
        textureBuffer, internalFormat);
}
//----------------------------------------------------------------------------
void GPUDevice::Texture3DUpdateFromPixelBuffer(Texture* texture, 
    PixelBuffer* pixelBuffer)
{
    (this->*_Texture3DUpdateFromPixelBuffer)(texture, pixelBuffer);
}
//----------------------------------------------------------------------------
TextureHandle* GPUDevice::Texture3DLoadFromSystemMemory(Texture* texture, 
    BufferInternalFormat internalFormat, int width, int height, int depth, 
    BufferFormat format, BufferComponentType type, void* pixels)
{
    return (this->*_Texture3DLoadFromSystemMemory)(texture, internalFormat, 
        width, height, depth, format, type, pixels);
}
//----------------------------------------------------------------------------
TextureHandle* GPUDevice::Tex2DArrayLoadFromSystemMemory(Texture* texture, 
    BufferInternalFormat internalFormat, int width, int height, int depth, 
	BufferFormat format, BufferComponentType type, bool mipmap, void* pixels)
{
    return (this->*_Tex2DArrayLoadFromSystemMemory)(texture, internalFormat, 
        width, height, depth, format, type, mipmap, pixels);
}
//----------------------------------------------------------------------------
void GPUDevice::Texture2DGetImageData(Texture* texture, void* dstPixels)
{
    (this->*_Texture2DGetImageData)(texture, dstPixels);
}
//----------------------------------------------------------------------------
void GPUDevice::Texture2DUpdateFromPixelBuffer(Texture* texture, 
    PixelBuffer* pixelBuffer)
{
    (this->*_Texture2DUpdateFromPixelBuffer)(texture, pixelBuffer);
}
//----------------------------------------------------------------------------
TextureHandle* GPUDevice::Texture2DLoadFromTextureBuffer(Texture* texture, 
    TextureBuffer* textureBuffer, BufferInternalFormat internalFormat)
{
    return (this->*_Texture2DLoadFromTextureBuffer)(texture, textureBuffer, 
        internalFormat);
}
//----------------------------------------------------------------------------
TextureHandle* GPUDevice::Texture2DLoadFromSystemMemory(Texture* texture, 
    BufferInternalFormat internalFormat, int width, int height, 
    BufferFormat format, BufferComponentType type, bool mipMap, 
    SamplerDesc* sampler, void* pixels)
{
    return (this->*_Texture2DLoadFromSystemMemory)(texture, internalFormat, 
        width, height, format, type, mipMap, sampler, pixels);
}
//----------------------------------------------------------------------------
void GPUDevice::Texture1DGetDataFromGPUMemory(Texture* texture, void* dstData)
{
    (this->*_Texture1DGetDataFromGPUMemory)(texture, dstData);
}
//----------------------------------------------------------------------------
void GPUDevice::TextureBindToSampler(Texture* texture, unsigned int index, 
    SamplerDesc* sampler)
{
    (this->*_TextureBindToSampler)(texture, index, sampler);
}
//----------------------------------------------------------------------------
void GPUDevice::TextureGenerateMipmap(Texture* texture)
{
    (this->*_TextureGenerateMipmap)(texture);
}
//----------------------------------------------------------------------------
void GPUDevice::TextureBindToImageUnit(Texture* texture, unsigned int unit, 
	bool layered, BufferAccess access)
{
	(this->*_TextureBindToImageUnit)(texture, unit, layered, access);
}
//----------------------------------------------------------------------------
void GPUDevice::Texture1DUpdateFromPixelBuffer(Texture* texture, 
    PixelBuffer* pixelBuffer)
{
    (this->*_Texture1DUpdateFromPixelBuffer)(texture, pixelBuffer);
}
//----------------------------------------------------------------------------
TextureHandle* GPUDevice::Texture1DLoadFromSystemMemory(Texture* texture, 
    BufferInternalFormat internalFormat, int width, BufferFormat format, 
    BufferComponentType type, void* pixels)
{
    return (this->*_Texture1DLoadFromSystemMemory)(texture, internalFormat, 
        width, format, type, pixels);
}
//----------------------------------------------------------------------------
void GPUDevice::DeleteTexture(Texture* texture)
{
    (this->*_DeleteTexture)(texture);
}
//----------------------------------------------------------------------------
void GPUDevice::SetProgramParameterInt(ShaderProgram* program,
    ShaderProgramParameter pname, int value)
{
    (this->*_SetProgramParameterInt)(program, pname, value);
}
//----------------------------------------------------------------------------
void GPUDevice::SetUniformValueFloat2(ShaderUniform* uniform, 
    const float* value)
{
    (this->*_SetUniformValueFloat2)(uniform, value);
}
//----------------------------------------------------------------------------
void GPUDevice::SetUniformValueFloat(ShaderUniform* uniform, float value)
{
    (this->*_SetUniformValueFloat)(uniform, value);
}
//----------------------------------------------------------------------------
void GPUDevice::SetUniformValueInt(ShaderUniform* uniform, int value)
{
    (this->*_SetUniformValueInt)(uniform, value);
}
//----------------------------------------------------------------------------
void GPUDevice::SetUniformValueUInt(ShaderUniform* uniform, 
    unsigned int value)
{
    (this->*_SetUniformValueUInt)(uniform, value);
}
//----------------------------------------------------------------------------
void GPUDevice::SetUniformValueVec3(ShaderUniform* uniform, 
    const glm::vec3* value)
{
    (this->*_SetUniformValueVec3)(uniform, value);
}
//----------------------------------------------------------------------------
void GPUDevice::SetUniformValueVec4(ShaderUniform* uniform,
	const glm::vec4* value)
{
	(this->*_SetUniformValueVec4)(uniform, value);
}
//----------------------------------------------------------------------------
void GPUDevice::SetUniformValueMat4(ShaderUniform* uniform, 
    const glm::mat4* value)
{
    (this->*_SetUniformValueMat4)(uniform, value);
}
//----------------------------------------------------------------------------
void GPUDevice::GetUniformLocation(ShaderProgram* program,
    ShaderUniform* uniform, const char* name)
{
    (this->*_GetUniformLocation)(program, uniform, name);
}
//----------------------------------------------------------------------------
void GPUDevice::DeletePassInfo(PassInfo* passInfo)
{
    (this->*_DeletePassInfo)(passInfo);
}
//----------------------------------------------------------------------------
PassInfoHandle* GPUDevice::CreatePassInfo(PassInfo* passInfo)
{
    return (this->*_CreatePassInfo)(passInfo);
}
//----------------------------------------------------------------------------
void GPUDevice::DisableProgram(ShaderProgram* program)
{
    (this->*_DisableProgram)(program);
}
//----------------------------------------------------------------------------
void GPUDevice::EnableProgram(ShaderProgram* program)
{
    (this->*_EnableProgram)(program);
}
//----------------------------------------------------------------------------
void GPUDevice::DeleteProgram(ShaderProgram* program)
{
    (this->*_DeleteProgram)(program);
}
//----------------------------------------------------------------------------
ShaderProgramHandle* GPUDevice::CreateProgram(ShaderProgram* program)
{
    return (this->*_CreateProgram)(program);
}
//----------------------------------------------------------------------------
void GPUDevice::DeleteShader(Shader* shader)
{
    (this->*_DeleteShader)(shader);
}
//----------------------------------------------------------------------------
ShaderHandle* GPUDevice::CreateShader(Shader* shader)
{
    return (this->*_CreateShader)(shader);
}
//----------------------------------------------------------------------------
void GPUDevice::Terminate()
{
    (this->*_Terminate)();
}
//----------------------------------------------------------------------------
void GPUDevice::Initialize(GPUDeviceDescription* deviceDesc)
{
    (this->*_Initialize)(deviceDesc);
}
//----------------------------------------------------------------------------
void GPUDevice::SetAnisFilterLevel(int maxAnisFilterLevel)
{
    (this->*_SetAnisFilterLevel)(maxAnisFilterLevel);
}
//----------------------------------------------------------------------------
void GPUDevice::GetMaxAnisFilterLevel(int* maxAnisFilterLevel)
{
    (this->*_GetMaxAnisFilterLevel)(maxAnisFilterLevel);
}
//----------------------------------------------------------------------------