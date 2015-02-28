//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "BufferTexture.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BufferTexture::BufferTexture()
{
    mType = TT_Texture1D;
}
//----------------------------------------------------------------------------
BufferTexture::~BufferTexture()
{
}
//----------------------------------------------------------------------------
bool BufferTexture::LoadFromTextureBuffer(GPUDevice* device, 
    TextureBuffer* textureBuffer, BufferInternalFormat internalFormat)
{
    if( mTextureHandle )
    {
        return false;
    }

    IsTextureBuffer = true;
    mInternalFormat = internalFormat;

    mTextureHandle = device->BufferTextureLoadFromTextureBuffer(this, 
        textureBuffer, internalFormat);

    return true;
}
//----------------------------------------------------------------------------