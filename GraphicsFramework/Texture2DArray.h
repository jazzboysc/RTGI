//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Texture2DArray_H
#define RTGI_Texture2DArray_H

#include "FrameworkCommon.h"
#include "Texture.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/22/2014
//----------------------------------------------------------------------------
class Texture2DArray : public Texture
{
public:
    Texture2DArray();
    ~Texture2DArray();

	void CreateRenderTarget(int width, int height, int depth, 
        RenderTargetFormat format);

	int Width, Height, Depth;
	bool IsRenderTarget;
	RenderTargetFormat RTFormat;

    virtual TextureType GetType();

private:
	GLint mInternalFormat;
	GLenum mFormat;
	GLenum mType;
};

typedef RefPointer<Texture2DArray> Texture2DArrayPtr;

}

#endif