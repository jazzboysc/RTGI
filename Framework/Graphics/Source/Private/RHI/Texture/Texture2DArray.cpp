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
	int height, int depth, BufferFormat format,bool generateMipMap)
{
	Width = width;
	Height = height;
    Depth = depth;
	IsRenderTarget = true;
	mFormat = format;

	switch (mFormat)
	{
	case BF_RGB:
		// TODO:
		RTGI_ASSERT( false );
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
        RTGI_ASSERT( false );
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

    mTextureHandle = device->Tex2DArrayLoadFromSystemMemory(
		this, mInternalFormat, Width, 
		Height, Depth, mFormat, mComponentType, generateMipMap, 0);
}
//--------------------------------------------------------------------------