//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_FrameBuffer_H
#define RTGI_FrameBuffer_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Texture2D.h"

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
		Texture2D** colorTextures, Texture2D* depthTexture = 0, 
		Texture2D* stencilTexture = 0);

	void Enable();
	void Disable();

private:
	GLuint mFBO;

	unsigned int mColorTextureCount;
	std::vector<Texture2D*> mColorTextures;
	GLenum* mColorBuffers;
	Texture2D* mDepthTexture;
	Texture2D* mStencilTexture;
};

typedef RefPointer<FrameBuffer> FrameBufferPtr;

}

#endif