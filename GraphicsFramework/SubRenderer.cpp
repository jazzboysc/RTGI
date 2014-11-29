//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "SubRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SubRenderer::SubRenderer()
{
    mFrameBuffer = new FrameBuffer();
}
//----------------------------------------------------------------------------
SubRenderer::~SubRenderer()
{
    for( int i = 0; i < (int)mRenderTargets.size(); ++i )
    {
        mRenderTargets[i] = 0;
    }
    mDepthTarget = 0;
    mFrameBuffer = 0;
}
//----------------------------------------------------------------------------
void SubRenderer::AddRenderTarget(const std::string& name, int width, 
    int height, Texture::TextureFormat format)
{
    assert(GetRenderTargetByName(name) == 0);
    Texture2D* texture = new Texture2D();
    texture->CreateRenderTarget(width, height, format);
    RenderTargetTexture* rt = new RenderTargetTexture(name, texture);
    mRenderTargets.push_back(rt);
}
//----------------------------------------------------------------------------
int SubRenderer::GetRenderTargetCount() const
{
    return (int)mRenderTargets.size();
}
//----------------------------------------------------------------------------
RenderTargetTexture* SubRenderer::GetRenderTarget(int i) const
{
    assert(i >= 0 && i < (int)mRenderTargets.size());
    return mRenderTargets[i];
}
//----------------------------------------------------------------------------
RenderTargetTexture* SubRenderer::GetRenderTargetByName(
    const std::string& name) const
{
    for( int i = 0; i < (int)mRenderTargets.size(); ++i )
    {
        if( mRenderTargets[i]->Name == name )
        {
            return mRenderTargets[i];
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
RenderTargetTexture* SubRenderer::GetDepthTarget() const
{
    return mDepthTarget;
}
//----------------------------------------------------------------------------
void SubRenderer::CreateFrameBuffer(int depthWidth, int depthHeight)
{
    int rtCount = (int)mRenderTargets.size();
    assert(rtCount > 0);
    Texture** renderTargets = new Texture*[rtCount];
    for( int i = 0; i < rtCount; ++i )
    {
        renderTargets[i] = (Texture*)mRenderTargets[i]->Texture;
    }

    Texture2D* depthTexture = new Texture2D();
    depthTexture->CreateRenderTarget(depthWidth, depthHeight, 
        Texture::TF_Depth);
    mDepthTarget = new RenderTargetTexture("Depth", depthTexture);

    mFrameBuffer->SetRenderTargets(rtCount, renderTargets, 
        (Texture*)mDepthTarget->Texture);
}
//----------------------------------------------------------------------------