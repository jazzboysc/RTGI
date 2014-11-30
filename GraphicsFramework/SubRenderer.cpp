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
    for( int i = 0; i < (int)mInputs.size(); ++i )
    {
        mInputs[i] = 0;
    }

    for( int i = 0; i < (int)mFrameBufferTargets.size(); ++i )
    {
        mFrameBufferTargets[i] = 0;
    }
    mDepthTarget = 0;
    mFrameBuffer = 0;
    mSceneManager = 0;
}
//----------------------------------------------------------------------------
void SubRenderer::AddFrameBufferTarget(const std::string& name, int width,
    int height, Texture::TextureFormat format)
{
    assert(GetFrameBufferTargetByName(name) == 0);
    Texture2D* texture = new Texture2D();
    texture->CreateRenderTarget(width, height, format);
    RendererOutput* rt = new RendererOutput(name, (BufferBase*)texture);
    mFrameBufferTargets.push_back(rt);
}
//----------------------------------------------------------------------------
int SubRenderer::GetFrameBufferTargetCount() const
{
    return (int)mFrameBufferTargets.size();
}
//----------------------------------------------------------------------------
RendererOutput* SubRenderer::GetFrameBufferTarget(int i) const
{
    assert(i >= 0 && i < (int)mFrameBufferTargets.size());
    return mFrameBufferTargets[i];
}
//----------------------------------------------------------------------------
RendererOutput* SubRenderer::GetFrameBufferTargetByName(
    const std::string& name) const
{
    for( int i = 0; i < (int)mFrameBufferTargets.size(); ++i )
    {
        if( mFrameBufferTargets[i]->Name == name )
        {
            return mFrameBufferTargets[i];
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
void SubRenderer::AddInputDependency(SubRenderer* producer, 
    const std::string& srcName, RendererDataView* view)
{
    assert(mInputs.size() < MAX_INPUT_DEPENDENCY_COUNT);
    RendererOutput* producerOutput = 
        producer->GetFrameBufferTargetByName(srcName);
    assert(producerOutput);

    RendererInput* consumerInput = new RendererInput(producerOutput->Name,
        producerOutput->OutputBuffer, view);
    mInputs.push_back(consumerInput);
}
//----------------------------------------------------------------------------
void SubRenderer::CreateFrameBuffer(int depthWidth, int depthHeight)
{
    int rtCount = (int)mFrameBufferTargets.size();
    assert(rtCount > 0);
    Texture** renderTargets = new Texture*[rtCount];
    for( int i = 0; i < rtCount; ++i )
    {
        renderTargets[i] = 
            (Texture*)(BufferBase*)mFrameBufferTargets[i]->OutputBuffer;
    }

    Texture2D* depthTexture = new Texture2D();
    depthTexture->CreateRenderTarget(depthWidth, depthHeight, 
        Texture::TF_Depth);
    mDepthTarget = new RendererOutput("Depth", (BufferBase*)depthTexture);

    mFrameBuffer->SetRenderTargets(rtCount, renderTargets, 
        (Texture*)(BufferBase*)mDepthTarget->OutputBuffer);
}
//----------------------------------------------------------------------------
void SubRenderer::AddGenericBufferTarget(const std::string& name, 
    RendererDataType bufferType, int size, BufferUsage usage)
{

}
//----------------------------------------------------------------------------
int SubRenderer::GetGenericBufferTargetCount() const
{
    return (int)mGenericBufferTargets.size();
}
//----------------------------------------------------------------------------
RendererOutput* SubRenderer::GetGenericBufferTarget(int i) const
{
    assert(i >= 0 && i < (int)mGenericBufferTargets.size());
    return mFrameBufferTargets[i];
}
//----------------------------------------------------------------------------
RendererOutput* SubRenderer::GetGenericBufferTargetByName(
    const std::string& name) const
{
    for( int i = 0; i < (int)mGenericBufferTargets.size(); ++i )
    {
        if( mGenericBufferTargets[i]->Name == name )
        {
            return mGenericBufferTargets[i];
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void SubRenderer::Render(int technique, int pass, 
    SubRendererOutput outputFlag)
{
    if( outputFlag & SRO_FrameBuffer )
    {
        assert(mFrameBuffer);
        mFrameBuffer->Enable();
    }

    int renderObjectCount = mSceneManager->GetRenderObjectCount();
    for( int i = 0; i < renderObjectCount; ++i )
    {
        RenderObject* renderObject = mSceneManager->GetRenderObject(i);
        renderObject->Render(technique, pass);
    }

    if( outputFlag & SRO_FrameBuffer )
    {
        mFrameBuffer->Disable();
    }
}
//----------------------------------------------------------------------------