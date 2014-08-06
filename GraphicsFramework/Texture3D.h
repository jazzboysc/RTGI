//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Texture3D_H
#define RTGI_Texture3D_H

#include "FrameworkCommon.h"
#include "Texture.h"
#include "PixelBuffer.h"
#include "TextureBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/05/2014
//----------------------------------------------------------------------------
class Texture3D : public Texture
{
public:
	Texture3D();
	~Texture3D();

	bool LoadFromSystemMemory(GLint internalFormat, GLsizei width, 
		GLsizei height, GLsizei depth, GLenum format, GLenum type, 
		void* pixels);

	int Width, Height, Depth;

private:
	GLint mInternalFormat;
	GLenum mFormat;
	GLenum mType;
};

typedef RefPointer<Texture3D> Texture3DPtr;

}

#endif