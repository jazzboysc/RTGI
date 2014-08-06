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

	mInternalFormat = internalFormat;
	mFormat = format;
	mType = type;

	glBindTexture(GL_TEXTURE_3D, 0);
	return true;
}
//----------------------------------------------------------------------------