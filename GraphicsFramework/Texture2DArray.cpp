//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Texture2DArray.h"
#include "vec.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Texture2DArray::Texture2DArray()
{
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
void Texture2DArray::CreateRenderTarget(int width, int height, int depth,
	TextureFormat format)
{
	Width = width;
	Height = height;
    Depth = depth;
	IsRenderTarget = true;
	RTFormat = format;

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D_ARRAY, mTexture);

	switch (RTFormat)
	{
	case RTGI::Texture::TF_RGB:
		// TODO:
		assert( false );
		break;

	case RTGI::Texture::TF_RGBF:
		mInternalFormat = GL_RGB32F_ARB;
		mFormat = GL_RGB;
		mType = GL_FLOAT;
		break;

    case RTGI::Texture::TF_RGBAF:
        mInternalFormat = GL_RGBA32F_ARB;
        mFormat = GL_RGBA;
        mType = GL_FLOAT;
        break;

	case RTGI::Texture::TF_R32UI:
#ifndef __APPLE__
		mInternalFormat = GL_R32UI;
		mFormat = GL_RED_INTEGER;
		mType = GL_UNSIGNED_INT;
#else
        assert( false );
#endif
		break;

    case RTGI::Texture::TF_R32F:
        mInternalFormat = GL_R32F;
        mFormat = GL_RED;
        mType = GL_FLOAT;
        break;

	case RTGI::Texture::TF_Depth:
		mInternalFormat = GL_DEPTH_COMPONENT24;
		mFormat = GL_DEPTH_COMPONENT;
		mType = GL_UNSIGNED_BYTE;
		break;

	default:
		break;
	}

	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, mInternalFormat, width, height, depth, 
        0, mFormat, mType, 0);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
//--------------------------------------------------------------------------
Texture::TextureType Texture2DArray::GetType()
{
    return Texture::TT_Texture2DArray;
}
//--------------------------------------------------------------------------