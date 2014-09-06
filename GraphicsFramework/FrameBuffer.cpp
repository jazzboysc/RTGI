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
	glGenFramebuffersEXT (1, &mFBO);
    mWidth = 0;
    mHeight = 0;
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
	Texture2D** colorTextures, Texture2D* depthTexture, 
	Texture2D* stencilTexture)
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

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFBO);

    // TODO:
    // Only support uniform size targets for now.
    mWidth = colorTextures[0]->Width;
    mHeight = colorTextures[0]->Height;

	for( unsigned int i = 0; i < colorTextureCount; ++i )
	{
		texture = colorTextures[i]->GetTexture();
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
			GL_COLOR_ATTACHMENT0_EXT + i, GL_TEXTURE_2D, texture, 0);
		mColorBuffers[i] = GL_COLOR_ATTACHMENT0_EXT + i;
		mColorTextures.push_back(colorTextures[i]);
	}

	if( depthTexture )
	{
		texture = depthTexture->GetTexture();
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
			GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, texture, 0);
	}
	mDepthTexture = depthTexture;

	if( stencilTexture )
	{
		texture = stencilTexture->GetTexture();
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, 
			GL_STENCIL_ATTACHMENT_EXT, GL_TEXTURE_2D, texture, 0);
	}
	mStencilTexture = stencilTexture;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
//--------------------------------------------------------------------------
static GLint oldViewport[4];
//--------------------------------------------------------------------------
void FrameBuffer::Enable()
{
    // Cache old viewport values and set new values.
    glGetIntegerv(GL_VIEWPORT, oldViewport);
    glViewport(0, 0, mWidth, mHeight);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, mFBO);
	glDrawBuffers(mColorTextureCount, mColorBuffers);
}
//--------------------------------------------------------------------------
void FrameBuffer::Disable()
{
    glViewport(oldViewport[0], oldViewport[1], oldViewport[2], 
        oldViewport[3]);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
//--------------------------------------------------------------------------