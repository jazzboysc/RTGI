//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Texture1D.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Texture1D::Texture1D()
{
	Width = 0;
}
//----------------------------------------------------------------------------
Texture1D::~Texture1D()
{
}
//----------------------------------------------------------------------------
bool Texture1D::LoadFromSystemMemory(GLint internalFormat, GLsizei width, 
	GLenum format, GLenum type, void* pixels)
{
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_1D, mTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, internalFormat, width, 0, format, type, 
		pixels);

	Width = width;
	mInternalFormat = internalFormat;
	mFormat = format;
	mType = type;

	glBindTexture(GL_TEXTURE_1D, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

	return true;
}
//----------------------------------------------------------------------------
void Texture1D::UpdateFromPixelBuffer(PixelBuffer* pixelBuffer)
{
	GLuint buffer = pixelBuffer->GetBuffer();
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
	glBindTexture(GL_TEXTURE_1D, mTexture);
	glTexImage1D(GL_TEXTURE_1D, 0, mInternalFormat, Width, 0, mFormat, mType, 
		0);
	glBindTexture(GL_TEXTURE_1D, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//--------------------------------------------------------------------------
void Texture1D::BindToImageUnit(GLuint unit, GLenum access)
{
#if defined(__APPLE__)
    assert( false );
#else
	glBindImageTexture(unit, mTexture, 0, GL_FALSE, 0, access, 
		mInternalFormat);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

#endif
}
//--------------------------------------------------------------------------
void Texture1D::CreateUniformRandomTexture(int sampleCount, int channelCount)
{
    Width = sampleCount;
    mType = GL_FLOAT;

    switch( channelCount )
    {
    case 1:
        mInternalFormat = GL_R32F;
        mFormat = GL_R;
        break;

    case 2:
        mInternalFormat = GL_RG32F;
        mFormat = GL_RG;
        break;

    case 3:
        mInternalFormat = GL_RGB32F;
        mFormat = GL_RGB;
        break;

    case 4:
        mInternalFormat = GL_RGBA32F;
        mFormat = GL_RGBA;
        break;

    default:
        assert(false);
        break;
    }

    float* pixels = new float[sampleCount * channelCount];
    for( int i = 0; i < sampleCount; ++i )
    {
        for( int j = 0; j < channelCount; ++j )
        {
            pixels[channelCount * i + j] = (float)UniformRandom();
        }
    }

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_1D, mTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, mInternalFormat, sampleCount, 0, mFormat, 
        mType, pixels);

    delete[] pixels;

    glBindTexture(GL_TEXTURE_1D, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//--------------------------------------------------------------------------
void Texture1D::GetDataFromGPUMemory(void* dstData)
{
    glBindTexture(GL_TEXTURE_1D, mTexture);
    glGetTexImage(GL_TEXTURE_1D, 0, mFormat, mType, dstData);
    glBindTexture(GL_TEXTURE_1D, 0);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//--------------------------------------------------------------------------
Texture::TextureType Texture1D::GetType()
{
    return Texture::TT_Texture1D;
}
//--------------------------------------------------------------------------