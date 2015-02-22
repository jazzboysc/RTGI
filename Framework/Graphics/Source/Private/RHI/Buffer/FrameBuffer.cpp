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

FrameBuffer::FrameBuffer(GPUDevice* device, int numBuffs, GLenum bufferType,
	int width, int height, BufferFormat format,
	bool enableAutoMipmaps /*= false*/)
{
// 	for (auto i = 0; i < numBuffs; ++i)
// 	{
// 		mColorTextures.push_back(new Texture2D());
// 	}
// 
// 	auto pDepthTex = new Texture2D();
// 	pColorTex->CreateRenderTarget(device, width, height, BF_RGBF);
// 	pDepthTex->CreateRenderTarget(device, width, height, BF_Depth);
// 	mReceiverGBuffer = new FrameBuffer(mDevice);
// 	Texture* receiverColorTextures[3] = { mReceiverPositionTexture, mReceiverNormalTexture, mReceiverColorTexture };
// 	mReceiverGBuffer->SetRenderTargets(3, receiverColorTextures, mReceiverDepthTexture);
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

    mFBO->Device->FrameBufferSetRenderTargets(this, 
        colorTextureCount, colorTextures, depthTexture, stencilTexture);
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