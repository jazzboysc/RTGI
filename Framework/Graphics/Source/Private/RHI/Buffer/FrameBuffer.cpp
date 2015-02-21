//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "FrameBuffer.h"

using namespace RTGI;

//--------------------------------------------------------------------------
FrameBuffer::FrameBuffer(GPUDevice* device)
{
    mFBO = GPU_DEVICE_FUNC(device, CreateFrameBuffer)(this);
    mWidth = 0;
    mHeight = 0;
    mDepth = 0;
	mColorTextureCount = 0;
	mColorBuffers = 0;
	mDepthTexture = 0;
	mStencilTexture = 0;
}
//--------------------------------------------------------------------------
FrameBuffer::~FrameBuffer()
{
    GPU_DEVICE_FUNC(mFBO->Device, DeleteFrameBuffer)(this);
	delete[] mColorBuffers;
}
//--------------------------------------------------------------------------
FBOHandle* FrameBuffer::GetFBOHandle()
{
	return mFBO;
}
//--------------------------------------------------------------------------
void FrameBuffer::SetRenderTargets(unsigned int colorTextureCount, 
    Texture** colorTextures, Texture* depthTexture, Texture* stencilTexture)
{
    assert(colorTextureCount > 0 && 
        colorTextureCount <= FBO_MAX_COLOR_TARGETS && colorTextures );


	mColorTextures.clear();
	if( mColorBuffers )
	{
		delete[] mColorBuffers;
	}

	mColorTextureCount = colorTextureCount;
	mColorBuffers = new unsigned int[colorTextureCount];

    // TODO:
    // Only support uniform size targets for now.
    TextureType textureType = colorTextures[0]->GetType();
    switch( textureType )
    {
    case TT_Texture1D:
        mWidth = ((Texture1D*)colorTextures[0])->Width;
        break;

    case TT_Texture2D:
        mWidth = ((Texture2D*)colorTextures[0])->Width;
        mHeight = ((Texture2D*)colorTextures[0])->Height;
        break;

    case TT_Texture2DArray:
        mWidth = ((Texture2DArray*)colorTextures[0])->Width;
        mHeight = ((Texture2DArray*)colorTextures[0])->Height;
        mDepth = ((Texture2DArray*)colorTextures[0])->Depth;
        break;

    default:
        assert( false );
        break;
    }

    GPU_DEVICE_FUNC(mFBO->Device, FrameBufferSetRenderTargets)(this, 
        colorTextureCount, colorTextures, depthTexture, stencilTexture);
}
//--------------------------------------------------------------------------
void FrameBuffer::Enable()
{
    GPU_DEVICE_FUNC(mFBO->Device, FrameBufferEnable)(this);
}
//--------------------------------------------------------------------------
void FrameBuffer::Disable()
{
    GPU_DEVICE_FUNC(mFBO->Device, FrameBufferDisable)(this);
}
//--------------------------------------------------------------------------