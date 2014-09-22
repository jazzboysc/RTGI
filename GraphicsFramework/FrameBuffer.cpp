//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "FrameBuffer.h"

using namespace RTGI;

//--------------------------------------------------------------------------
FrameBuffer::FrameBuffer()
{
	glGenFramebuffersEXT(1, &mFBO);
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
	glDeleteFramebuffersEXT(1, &mFBO);
	delete[] mColorBuffers;
}
//--------------------------------------------------------------------------
GLuint FrameBuffer::GetFBO()
{
	return mFBO;
}
//--------------------------------------------------------------------------
void FrameBuffer::SetRenderTargets(unsigned int colorTextureCount, 
	Texture** colorTextures, Texture* depthTexture, Texture* stencilTexture)
{
	assert( colorTextureCount > 0 && colorTextureCount <= 16 && 
		colorTextures );

	mColorTextures.clear();
	if( mColorBuffers )
	{
		delete[] mColorBuffers;
	}

	GLuint texture;
	mColorTextureCount = colorTextureCount;
	mColorBuffers = new GLenum[colorTextureCount];

	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);

    // TODO:
    // Only support uniform size targets for now.
    Texture::TextureType textureType = colorTextures[0]->GetType();
    switch( textureType )
    {
    case Texture::TT_Texture1D:
        mWidth = ((Texture1D*)colorTextures[0])->Width;
        break;

    case Texture::TT_Texture2D:
        mWidth = ((Texture2D*)colorTextures[0])->Width;
        mHeight = ((Texture2D*)colorTextures[0])->Height;
        break;

    case Texture::TT_Texture2DArray:
        mWidth = ((Texture2DArray*)colorTextures[0])->Width;
        mHeight = ((Texture2DArray*)colorTextures[0])->Height;
        mDepth = ((Texture2DArray*)colorTextures[0])->Depth;
        break;

    default:
        assert( false );
        break;
    }

	for( unsigned int i = 0; i < colorTextureCount; ++i )
	{
		texture = colorTextures[i]->GetTexture();
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, 
            texture, 0);
		mColorBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
		mColorTextures.push_back(colorTextures[i]);
	}

	if( depthTexture )
	{
		texture = depthTexture->GetTexture();
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture, 
            0);
	}
	mDepthTexture = depthTexture;

	if( stencilTexture )
	{
		texture = stencilTexture->GetTexture();
		glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, texture, 
            0);
	}
	mStencilTexture = stencilTexture;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//--------------------------------------------------------------------------
static GLint oldViewport[4];
//--------------------------------------------------------------------------
void FrameBuffer::Enable()
{
    // Cache old viewport values and set new values.
    glGetIntegerv(GL_VIEWPORT, oldViewport);
    glViewport(0, 0, mWidth, mHeight);

	glBindFramebuffer(GL_FRAMEBUFFER, mFBO);
	glDrawBuffers(mColorTextureCount, mColorBuffers);
}
//--------------------------------------------------------------------------
void FrameBuffer::Disable()
{
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], 
        oldViewport[3]);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
//--------------------------------------------------------------------------