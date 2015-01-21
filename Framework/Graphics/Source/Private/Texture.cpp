//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Texture.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Texture::Texture()
    :
    IsRenderTarget(false),
    IsHDRTexture(false),
    IsTextureBuffer(false),
    mTextureHandle(0),
    mFormat(TF_Unknown),
    mInternalFormat(TIF_Unknown),
    mComponentType(TCT_Unknown)
{
}
//----------------------------------------------------------------------------
Texture::~Texture()
{
    GPU_DEVICE_FUNC(mTextureHandle->Device, DeleteTexture)(this);
    delete mTextureHandle;
}
//----------------------------------------------------------------------------
TextureHandle* Texture::GetTextureHandle() const
{
    return mTextureHandle;
}
//----------------------------------------------------------------------------
TextureFormat Texture::GetFormat() const
{
    return mFormat;
}
//----------------------------------------------------------------------------
TextureInternalFormat Texture::GetInternalFormat() const
{
    return mInternalFormat;
}
//----------------------------------------------------------------------------
TextureComponentType Texture::GetComponentType() const
{
    return mComponentType;
}
//----------------------------------------------------------------------------
TextureType Texture::GetType() const
{
    return mType;
}
//----------------------------------------------------------------------------
void Texture::BindToImageUnit(unsigned int unit, BufferAccess access)
{
    GPU_DEVICE_FUNC(mTextureHandle->Device, TextureBindToImageUnit)(this,
        unit, access);
}
//--------------------------------------------------------------------------
void Texture::BindToSampler(unsigned int index, SamplerDesc* sampler)
{
    GPU_DEVICE_FUNC(mTextureHandle->Device, TextureBindToSampler)(this, 
        index, sampler);
}
//--------------------------------------------------------------------------