//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Texture3D.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Texture3D::Texture3D()
{
    mType = TT_Texture3D;
	Width = 0;
	Height = 0;
	Depth = 0;
}
//----------------------------------------------------------------------------
Texture3D::~Texture3D()
{
}
//----------------------------------------------------------------------------
bool Texture3D::LoadFromSystemMemory(GPUDevice* device,
    TextureInternalFormat internalFormat, int width, int height, int depth,
    TextureFormat format, TextureComponentType type, void* pixels)
{
    if( mTextureHandle )
    {
        assert(false);
        return false;
    }

    Width = width;
    Height = height;
    Depth = depth;
    mInternalFormat = internalFormat;
    mFormat = format;
    mComponentType = type;

    mTextureHandle = GPU_DEVICE_FUNC(device, Texture3DLoadFromSystemMemory)(
        this, mInternalFormat, Width, Height, Depth, mFormat, mComponentType,
        pixels);

	return true;
}
//----------------------------------------------------------------------------
void Texture3D::CreateRenderTarget(GPUDevice* device, int width, int height, 
    int depth, TextureFormat format)
{
    if( mTextureHandle )
    {
        assert(false);
        return;
    }

    Width = width;
    Height = height;
    Depth = depth;
    IsRenderTarget = true;
    mFormat = format;

    switch( mFormat )
    {
    case TF_RGB:
        mInternalFormat = TIF_RGB8;
        mComponentType = TCT_Unsigned_Byte;
        break;

    case TF_RGBA:
        mInternalFormat = TIF_RGBA8;
        mComponentType = TCT_Unsigned_Byte;
        break;

    case TF_RGBF:
        mInternalFormat = TIF_RGB32F;
        mComponentType = TCT_Float;
        break;

    case TF_RGBAF:
        mInternalFormat = TIF_RGBA32F;
        mComponentType = TCT_Float;
        break;

    case TF_R32UI:
#ifndef __APPLE__
        mInternalFormat = TIF_R32UI;
        mComponentType = TCT_Unsigned_Int;
#else
        assert(false);
#endif
        break;

    case TF_R32F:
        mInternalFormat = TIF_R32F;
        mComponentType = TCT_Float;
        break;

    case TF_Depth:
        mInternalFormat = TIF_Depth24;
        mComponentType = TCT_Unsigned_Byte;
        break;

    default:
        break;
    }

    mTextureHandle = GPU_DEVICE_FUNC(device, Texture3DLoadFromSystemMemory)(
        this, mInternalFormat, Width, Height, Depth, mFormat, mComponentType,
        0);
}
//----------------------------------------------------------------------------
void Texture3D::UpdateFromPixelBuffer(PixelBuffer* pixelBuffer)
{
    assert(mTextureHandle);
    GPU_DEVICE_FUNC(mTextureHandle->Device, Texture3DUpdateFromPixelBuffer)(
        this, pixelBuffer);
}
//--------------------------------------------------------------------------