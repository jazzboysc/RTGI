//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Texture2DArray.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Texture2DArray::Texture2DArray()
{
    mType = TT_Texture2DArray;
	Width = 0;
	Height = 0;
    Depth = 0;
	IsRenderTarget = false;
}
//----------------------------------------------------------------------------
Texture2DArray::~Texture2DArray()
{
}
//----------------------------------------------------------------------------
void Texture2DArray::CreateRenderTarget(GPUDevice* device, int width, 
    int height, int depth, TextureFormat format)
{
	Width = width;
	Height = height;
    Depth = depth;
	IsRenderTarget = true;
	mFormat = format;

	switch (mFormat)
	{
	case TF_RGB:
		// TODO:
		assert( false );
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
        assert( false );
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

    mTextureHandle = GPU_DEVICE_FUNC(device,
        Tex2DArrayLoadFromSystemMemory)(this, mInternalFormat, Width, 
        Height, Depth, mFormat, mComponentType, 0);
}
//--------------------------------------------------------------------------