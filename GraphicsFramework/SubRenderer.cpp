//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "SubRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SubRenderer::SubRenderer(SceneManager* sceneManager)
{
    mFrameBuffer = new FrameBuffer();
    mSceneManager = sceneManager;
}
//----------------------------------------------------------------------------
SubRenderer::~SubRenderer()
{
    for( int i = 0; i < MAX_INPUT_DEPENDENCY_COUNT; ++i )
    {
        mInputs[i] = 0;
    }

    for( int i = 0; i < (int)mRenderTargets.size(); ++i )
    {
        mRenderTargets[i] = 0;
    }
    mDepthTarget = 0;
    mFrameBuffer = 0;
    mSceneManager = 0;
}
//----------------------------------------------------------------------------
void SubRenderer::AddRenderTarget(const std::string& name, int width, 
    int height, Texture::TextureFormat format)
{
    assert(GetRenderTargetByName(name) == 0);
    Texture2D* texture = new Texture2D();
    texture->CreateRenderTarget(width, height, format);
    RendererOutput* rt = new RendererOutput(name, (BufferBase*)texture);
    mRenderTargets.push_back(rt);
}
//----------------------------------------------------------------------------
int SubRenderer::GetRenderTargetCount() const
{
    return (int)mRenderTargets.size();
}
//----------------------------------------------------------------------------
RendererOutput* SubRenderer::GetRenderTarget(int i) const
{
    assert(i >= 0 && i < (int)mRenderTargets.size());
    return mRenderTargets[i];
}
//----------------------------------------------------------------------------
RendererOutput* SubRenderer::GetRenderTargetByName(
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
RendererOutput* SubRenderer::GetDepthTarget() const
{
    return mDepthTarget;
}
//----------------------------------------------------------------------------
void SubRenderer::SetInputDependency(SubRenderer* producer, 
    const std::string& name, int slot)
{
    assert(slot >= 0 && slot < MAX_INPUT_DEPENDENCY_COUNT);
    RendererOutput* producerOutput = producer->GetRenderTargetByName(name);
    assert(producerOutput);
    mInputs[slot] = producerOutput;
}
//----------------------------------------------------------------------------
void SubRenderer::CreateFrameBuffer(int depthWidth, int depthHeight)
{
    int rtCount = (int)mRenderTargets.size();
    assert(rtCount > 0);
    Texture** renderTargets = new Texture*[rtCount];
    for( int i = 0; i < rtCount; ++i )
    {
        renderTargets[i] = 
            (Texture*)(BufferBase*)mRenderTargets[i]->OutputBuffer;
    }

    Texture2D* depthTexture = new Texture2D();
    depthTexture->CreateRenderTarget(depthWidth, depthHeight, 
        Texture::TF_Depth);
    mDepthTarget = new RendererOutput("Depth", (BufferBase*)depthTexture);

    mFrameBuffer->SetRenderTargets(rtCount, renderTargets, 
        (Texture*)(BufferBase*)mDepthTarget->OutputBuffer);
}
//----------------------------------------------------------------------------
void SubRenderer::Render(int technique, int pass)
{
    int renderObjectCount = mSceneManager->GetRenderObjectCount();
    for( int i = 0; i < renderObjectCount; ++i )
    {
        RenderObject* renderObject = mSceneManager->GetRenderObject(i);
        renderObject->Render(technique, pass);
    }
}
//----------------------------------------------------------------------------