//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_FrameBuffer_H
#define RTGI_FrameBuffer_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Texture1D.h"
#include "Texture2D.h"
#include "Texture3D.h"
#include "TextureCube.h"
#include "Texture2DArray.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/22/2014
//----------------------------------------------------------------------------
class FrameBuffer : public RefObject
{
public:
	FrameBuffer();
	~FrameBuffer();

	GLuint GetFBO();

	void SetRenderTargets(unsigned int colorTextureCount, 
		Texture** colorTextures, Texture* depthTexture = 0, 
		Texture* stencilTexture = 0);

	void Enable();
	void Disable();

private:
	GLuint mFBO;

    GLsizei mWidth, mHeight, mDepth;
	unsigned int mColorTextureCount;
	std::vector<Texture*> mColorTextures;
	GLenum* mColorBuffers;
	Texture* mDepthTexture;
	Texture* mStencilTexture;
};

typedef RefPointer<FrameBuffer> FrameBufferPtr;

}

#endif