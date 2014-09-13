//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
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
}
//--------------------------------------------------------------------------
void Texture1D::BindToImageUnit(GLuint unit, GLenum access)
{
#if defined(__APPLE__)
    assert( false );
#else
	glBindImageTexture(unit, mTexture, 0, GL_FALSE, 0, access, 
		mInternalFormat);
#endif
}
//--------------------------------------------------------------------------
void Texture1D::CreateUniformRandomTextureRG(int sampleCount)
{
    float* pixels = new float[sampleCount * 2];
    for( int i = 0; i < sampleCount; ++i )
    {
        pixels[2 * i    ] = (float)UniformRandom();
        pixels[2 * i + 1] = (float)UniformRandom();
    }

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_1D, mTexture);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RG32F, sampleCount, 0, GL_RG, GL_FLOAT,
        pixels);

    delete[] pixels;

    Width = sampleCount;
    mInternalFormat = GL_RG32F;
    mFormat = GL_RG;
    mType = GL_FLOAT;

    glBindTexture(GL_TEXTURE_1D, 0);
}
//--------------------------------------------------------------------------