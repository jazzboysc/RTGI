//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "SubRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BufferBase* CreateStructuredBuffer(int size, BufferUsage usage)
{
    StructuredBuffer* buffer = new StructuredBuffer();
    buffer->ReserveDeviceResource(size, usage);
    return (BufferBase*)buffer;
}
//----------------------------------------------------------------------------
BufferBase* CreateAtomicCounterBuffer(int size, BufferUsage usage)
{
    AtomicCounterBuffer* buffer = new AtomicCounterBuffer();
    buffer->ReserveDeviceResource(size, usage);
    return (BufferBase*)buffer;
}
//----------------------------------------------------------------------------
BufferBase* CreateUniformBuffer(int size, BufferUsage usage)
{
    UniformBuffer* buffer = new UniformBuffer();
    buffer->ReserveDeviceResource(size, usage);
    return (BufferBase*)buffer;
}
//----------------------------------------------------------------------------
BufferBase* CreateDispatchIndirectBuffer(int size, BufferUsage usage)
{
    DispatchIndirectBuffer* buffer = new DispatchIndirectBuffer();
    buffer->ReserveDeviceResource(size, usage);
    return (BufferBase*)buffer;
}
//----------------------------------------------------------------------------
BufferBase* CreatePixelBuffer(int size, BufferUsage usage)
{
    PixelBuffer* buffer = new PixelBuffer();
    buffer->ReserveDeviceResource(size, usage);
    return (BufferBase*)buffer;
}
//----------------------------------------------------------------------------
BufferBase* CreateTextureBuffer(int size, BufferUsage usage)
{
    TextureBuffer* buffer = new TextureBuffer();
    buffer->ReserveDeviceResource(size, usage);
    return (BufferBase*)buffer;
}
//----------------------------------------------------------------------------

SubRendererCreateRendererData SubRenderer::msFactoryFunctions[6] =
{
    CreateStructuredBuffer,
    CreateAtomicCounterBuffer,
    CreateUniformBuffer,
    CreateDispatchIndirectBuffer,
    CreatePixelBuffer,
    CreateTextureBuffer
};

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
void SubRenderer::SetSceneManager(SceneManager* sceneManager)
{
    mSceneManager = sceneManager;
}
//----------------------------------------------------------------------------
SceneManager* SubRenderer::GetSceneManager() const
{
    return mSceneManager;
}
//----------------------------------------------------------------------------
void SubRenderer::AddFrameBufferTarget(const std::string& name, int width,
    int height, Texture::TextureFormat format)
{
    assert(GetFrameBufferTargetByName(name) == 0);
    Texture2D* texture = new Texture2D();
    texture->CreateRenderTarget(width, height, format);
    RendererOutput* ro = new RendererOutput(name, (BufferBase*)texture);
    mFrameBufferTargets.push_back(ro);
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
    RendererDataType bufferType, int size, BufferUsage usage, 
    BindingFlag flag, unsigned int binding)
{
    assert(GetGenericBufferTargetByName(name) == 0);
    int functionIndex = IntLog2((int)bufferType) - 2;
    BufferBase* genericBufferTarget = msFactoryFunctions[functionIndex](size, 
        usage);

    RendererOutput* ro = new RendererOutput(name, genericBufferTarget, false, 
        flag, binding);
    mGenericBufferTargets.push_back(ro);
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
    // Enable renderer inputs.
    for( int i = 0; i < (int)mInputs.size(); ++i )
    {
        mInputs[i]->Enable();
    }

    // Enable renderer framebuffer outputs.
    if( outputFlag & SRO_FrameBuffer )
    {
        assert(mFrameBuffer);
        mFrameBuffer->Enable();
    }

    // Enable renderer generic buffer outputs.
    if( outputFlag & SRO_GenericBuffer )
    {
        for( int i = 0; i < (int)mGenericBufferTargets.size(); ++i )
        {
            mGenericBufferTargets[i]->Enable();
        }
    }

    // Render scene inputs.
    assert(mSceneManager);
    int renderObjectCount = mSceneManager->GetRenderObjectCount();
    for( int i = 0; i < renderObjectCount; ++i )
    {
        RenderObject* renderObject = mSceneManager->GetRenderObject(i);
        renderObject->Render(technique, pass);
    }

    // Disable renderer inputs.
    for( int i = 0; i < (int)mInputs.size(); ++i )
    {
        mInputs[i]->Disable();
    }

    // Disable renderer framebuffer outputs.
    if( outputFlag & SRO_FrameBuffer )
    {
        mFrameBuffer->Disable();
    }

    // Disable renderer generic buffer outputs.
    if( outputFlag & SRO_GenericBuffer )
    {
        for( int i = 0; i < (int)mGenericBufferTargets.size(); ++i )
        {
            mGenericBufferTargets[i]->Disable();
        }
    }
}
//----------------------------------------------------------------------------