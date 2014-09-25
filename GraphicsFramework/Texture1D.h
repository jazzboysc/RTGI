//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Texture1D_H
#define RTGI_Texture1D_H

#include "FrameworkCommon.h"
#include "Texture.h"
#include "PixelBuffer.h"
#include "TextureBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/10/2014
//----------------------------------------------------------------------------
class Texture1D : public Texture
{
public:
	Texture1D();
	~Texture1D();

	bool LoadFromSystemMemory(GLint internalFormat, GLsizei width, 
		GLenum format, GLenum type, void* pixels);

		// Update a render target from pixel buffer.
	void UpdateFromPixelBuffer(PixelBuffer* pixelBuffer);

	void BindToImageUnit(GLuint unit, GLenum access);

    void CreateUniformRandomTexture(int sampleCount, int channelCount);

    virtual TextureType GetType();

	int Width;

private:
	GLint mInternalFormat;
	GLenum mFormat;
	GLenum mType;
};

typedef RefPointer<Texture1D> Texture1DPtr;

}

#endif