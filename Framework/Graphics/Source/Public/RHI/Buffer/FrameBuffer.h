//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_FrameBuffer_H
#define RTGI_FrameBuffer_H

#include "RefObject.h"
#include "GPUResource.h"
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
    FrameBuffer(GPUDevice* device);
	~FrameBuffer();

    FBOHandle* GetFBOHandle();

    void SetRenderTargets(unsigned int colorTextureCount,
		Texture** colorTextures, Texture* depthTexture = 0, 
		Texture* stencilTexture = 0);

	void Enable();
	void Disable();

    enum { FBO_MAX_COLOR_TARGETS = 16 };

private:
    FBOHandle* mFBO;

// Internal.
public:
    int mWidth, mHeight, mDepth;
	unsigned int mColorTextureCount;
	std::vector<Texture*> mColorTextures;
	unsigned int* mColorBuffers;
	Texture* mDepthTexture;
	Texture* mStencilTexture;
};

typedef RefPointer<FrameBuffer> FrameBufferPtr;

}

#endif