//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Texture.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Texture::Texture()
    :
    IsRenderTarget(false),
    IsHDRTexture(false),
    IsTextureBuffer(false)
{
}
//----------------------------------------------------------------------------
Texture::~Texture()
{
	glDeleteTextures(1, &mTexture);
}
//----------------------------------------------------------------------------
GLuint Texture::GetTexture() const
{
	return mTexture;
}
//----------------------------------------------------------------------------