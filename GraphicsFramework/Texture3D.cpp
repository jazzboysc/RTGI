//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Texture3D.h"
#include "vec.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Texture3D::Texture3D()
{
	Width = 0;
	Height = 0;
	Depth = 0;
}
//----------------------------------------------------------------------------
Texture3D::~Texture3D()
{
}
//----------------------------------------------------------------------------
bool Texture3D::LoadFromSystemMemory(GLint internalFormat, GLsizei width, 
	GLsizei height, GLsizei depth, GLenum format, GLenum type, void* pixels)
{
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_3D, mTexture);
	glTexImage3D(GL_TEXTURE_3D, 0, internalFormat, width, height, depth, 0, 
		format, type, pixels);

	Width = width;
	Height = height;
	Depth = depth;
	mInternalFormat = internalFormat;
	mFormat = format;
	mType = type;

	glBindTexture(GL_TEXTURE_3D, 0);
	return true;
}
//----------------------------------------------------------------------------
void Texture3D::CreateRenderTarget(int width, int height, int depth,
    TextureFormat format)
{
    Width = width;
    Height = height;
    Depth = depth;
    IsRenderTarget = true;
    RTFormat = format;

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_3D, mTexture);

    switch( RTFormat )
    {
    case RTGI::Texture::TF_RGB:
        mInternalFormat = GL_RGB8;
        mFormat = GL_RGB;
        mType = GL_UNSIGNED_BYTE;
        break;

    case RTGI::Texture::TF_RGBA:
        mInternalFormat = GL_RGBA8;
        mFormat = GL_RGBA;
        mType = GL_UNSIGNED_BYTE;
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
        assert(false);
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

    glTexImage3D(GL_TEXTURE_3D, 0, mInternalFormat, width, height, depth, 0,
        mFormat, mType, 0);

    glBindTexture(GL_TEXTURE_3D, 0);
}
//----------------------------------------------------------------------------
void Texture3D::UpdateFromPixelBuffer(PixelBuffer* pixelBuffer)
{
	GLuint buffer = pixelBuffer->GetBuffer();
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
	glBindTexture(GL_TEXTURE_3D, mTexture);
	glTexImage3D(GL_TEXTURE_3D, 0, mInternalFormat, Width, Height, Depth, 0, 
		mFormat, mType, 0);
	glBindTexture(GL_TEXTURE_3D, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
//--------------------------------------------------------------------------
void Texture3D::BindToImageUnit(GLuint unit, GLenum access)
{
#if defined(__APPLE__)
    assert( false );
#else
	glBindImageTexture(unit, mTexture, 0, GL_FALSE, 0, access, 
		mInternalFormat);
#endif
}
//--------------------------------------------------------------------------
Texture::TextureType Texture3D::GetType()
{
    return Texture::TT_Texture3D;
}
//--------------------------------------------------------------------------