//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "FrameBuffer.h"

using namespace RTGI;

//--------------------------------------------------------------------------
FrameBuffer::FrameBuffer(GPUDevice* device)
{
    mFBO = device->CreateFrameBuffer(this);
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
    mFBO->Device->DeleteFrameBuffer(this);
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
    if( colorTextureCount > 0 )
    {
        RTGI_ASSERT(colorTextureCount <= FBO_MAX_COLOR_TARGETS && 
            colorTextures);

        mColorTextures.clear();
        if( mColorBuffers )
        {
            delete[] mColorBuffers;
        }

        mColorTextureCount = colorTextureCount;
        mColorBuffers = new unsigned int[colorTextureCount];

        // Only support uniform size targets.
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
            RTGI_ASSERT(false);
            break;
        }
    }
    mDepthTexture = depthTexture;
    mStencilTexture = stencilTexture;

    mFBO->Device->FrameBufferSetRenderTargets(this, 
        colorTextureCount, colorTextures, depthTexture, stencilTexture);
}
//--------------------------------------------------------------------------
void FrameBuffer::ReserveColorTargets(unsigned int colorTextureCount, 
    int width, int height, int depth)
{
    if( colorTextureCount > 0 )
    {
        RTGI_ASSERT(colorTextureCount <= FBO_MAX_COLOR_TARGETS);

        mWidth = width;
        mHeight = height;
        mDepth = depth;

        mColorTextures.clear();
        mColorTextures.reserve(colorTextureCount);
        if( mColorBuffers )
        {
            delete[] mColorBuffers;
        }
        mColorBuffers = new unsigned int[colorTextureCount];
    }
}
//--------------------------------------------------------------------------
void FrameBuffer::SetColorTarget(unsigned int attachPoint, 
    Texture* colorTexture)
{
    RTGI_ASSERT(attachPoint >= 0 && attachPoint <
        (unsigned int)mColorTextures.size());
    
    mColorTextures[attachPoint] = colorTexture;
    mFBO->Device->FrameBufferSetColorTarget(this, attachPoint, colorTexture);
}
//--------------------------------------------------------------------------
void FrameBuffer::SetDepthTarget(Texture* depthTexture)
{
    mDepthTexture = depthTexture;
    mFBO->Device->FrameBufferSetDepthTarget(this, mDepthTexture);
}
//--------------------------------------------------------------------------
void FrameBuffer::SetStencilTarget(Texture* stencilTexture)
{
    mStencilTexture = stencilTexture;
    mFBO->Device->FrameBufferSetStencilTarget(this, mStencilTexture);
}
//--------------------------------------------------------------------------
void FrameBuffer::Enable()
{
    mFBO->Device->FrameBufferEnable(this);
}
//--------------------------------------------------------------------------
void FrameBuffer::Disable()
{
    mFBO->Device->FrameBufferDisable(this);
}
//--------------------------------------------------------------------------