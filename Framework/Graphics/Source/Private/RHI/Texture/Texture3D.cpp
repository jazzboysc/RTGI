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
    BufferInternalFormat internalFormat, int width, int height, int depth,
    BufferFormat format, BufferComponentType type, void* pixels)
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
    int depth, BufferFormat format)
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
    case BF_RGB:
        mInternalFormat = BIF_RGB8;
        mComponentType = BCT_Unsigned_Byte;
        break;

    case BF_RGBA:
        mInternalFormat = BIF_RGBA8;
        mComponentType = BCT_Unsigned_Byte;
        break;

    case BF_RGBF:
        mInternalFormat = BIF_RGB32F;
        mComponentType = BCT_Float;
        break;

    case BF_RGBAF:
        mInternalFormat = BIF_RGBA32F;
        mComponentType = BCT_Float;
        break;

    case BF_R32UI:
#ifndef __APPLE__
        mInternalFormat = BIF_R32UI;
        mComponentType = BCT_Unsigned_Int;
#else
        assert(false);
#endif
        break;

    case BF_R32F:
        mInternalFormat = BIF_R32F;
        mComponentType = BCT_Float;
        break;

    case BF_Depth:
        mInternalFormat = BIF_Depth24;
        mComponentType = BCT_Unsigned_Byte;
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