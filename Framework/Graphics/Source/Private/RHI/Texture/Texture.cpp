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
    HasMipMap(false),
    mTextureHandle(0),
    mFormat(BF_Unknown),
    mInternalFormat(BIF_Unknown),
    mComponentType(BCT_Unknown)
{
}
//----------------------------------------------------------------------------
Texture::~Texture()
{
    if( mTextureHandle )
    {
        mTextureHandle->Device->DeleteTexture(this);
        delete mTextureHandle;
    }
}
//----------------------------------------------------------------------------
void Texture::BindToImageUnit(unsigned int unit, BufferAccess access, bool layered)
{
    mTextureHandle->Device->TextureBindToImageUnit(this, unit, layered, access);
}
//--------------------------------------------------------------------------
void Texture::BindToSampler(unsigned int index, SamplerDesc* sampler)
{
    if( !HasMipMap && sampler->MinFilter == FT_Linear_Linear )
    {
        sampler->MinFilter = FT_Linear;
    }

    mTextureHandle->Device->TextureBindToSampler(this, index, sampler);
}
//--------------------------------------------------------------------------
void Texture::GenerateMipmap()
{
    RTGI_ASSERT(HasMipMap);

}
//--------------------------------------------------------------------------