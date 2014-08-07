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