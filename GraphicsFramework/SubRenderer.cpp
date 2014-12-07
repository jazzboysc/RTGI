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
SubRenderer::SubRenderer(RenderSet* renderSet)
{
    mFrameBuffer = new FrameBuffer();
    mRenderSet = renderSet;
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
    mRenderSet = 0;
    mTimer = 0;
}
//----------------------------------------------------------------------------
void SubRenderer::SetRenderSet(RenderSet* renderSet)
{
    mRenderSet = renderSet;
}
//----------------------------------------------------------------------------
RenderSet* SubRenderer::GetRenderSet() const
{
    return mRenderSet;
}
//----------------------------------------------------------------------------
void SubRenderer::AddFrameBufferTarget(const std::string& name, int width,
    int height, int depth, Texture::TextureType type, 
    Texture::TextureFormat format)
{
    assert(GetFrameBufferTargetByName(name) == 0);

    BufferBase* texture = 0;
    switch( type )
    {
    case Texture::TT_Texture2D:
        texture = new Texture2D();
        ((Texture2D*)texture)->CreateRenderTarget(width, height, format);
        break;
    case Texture::TT_Texture2DArray:
        texture = new Texture2DArray();
        ((Texture2DArray*)texture)->CreateRenderTarget(width, height, depth, 
            format);
        break;
    default:
        assert(false);
        break;
    }

    RendererOutput* ro = new RendererOutput(name, texture);
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
    const std::string& srcName, RendererInputDataView* view)
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
RendererInput* SubRenderer::GetInputDependency(int i) const
{
    assert(i >= 0 && i < (int)mInputs.size());
    return mInputs[i];
}
//----------------------------------------------------------------------------
void SubRenderer::ClearInputDependency()
{
    for( int i = 0; i < (int)mInputs.size(); ++i )
    {
        mInputs[i] = 0;
    }
    mInputs.clear();
}
//----------------------------------------------------------------------------
void SubRenderer::CreateFrameBuffer(int depthWidth, int depthHeight, 
    int depthCount, Texture::TextureType depthType)
{
    int rtCount = (int)mFrameBufferTargets.size();
    assert(rtCount > 0);
    Texture** renderTargets = new Texture*[rtCount];
    for( int i = 0; i < rtCount; ++i )
    {
        renderTargets[i] = 
            (Texture*)(BufferBase*)mFrameBufferTargets[i]->OutputBuffer;
    }

    BufferBase* depthTexture = 0;
    switch( depthType )
    {
    case Texture::TT_Texture2D:
        depthTexture = new Texture2D();
        ((Texture2D*)depthTexture)->CreateRenderTarget(depthWidth, depthHeight,
            Texture::TF_Depth);
        break;

    case Texture::TT_Texture2DArray:
        depthTexture = new Texture2DArray();
        ((Texture2DArray*)depthTexture)->CreateRenderTarget(depthWidth, 
            depthHeight, depthCount, Texture::TF_Depth);
        break;

    default:
        assert(false);
        break;
    }
    mDepthTarget = new RendererOutput("Depth", depthTexture);

    mFrameBuffer->SetRenderTargets(rtCount, renderTargets, 
        (Texture*)(BufferBase*)mDepthTarget->OutputBuffer);
    delete[] renderTargets;
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
    SubRendererOutput outputFlag, PipelineStateBlock* psb, Camera* camera)
{
    assert(mTimer);
    mTimer->Start();

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

    // Apply PSB.
    if( psb )
    {
        ApplyPipelineStateBlock(psb);
    }

    // Render scene inputs.
    OnRender(technique, pass, camera);

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

    mTimer->Stop();
}
//----------------------------------------------------------------------------
void SubRenderer::OnRender(int technique, int pass, Camera* camera)
{
    assert(mRenderSet);
    int renderObjectCount = mRenderSet->GetRenderObjectCount();
    if( camera )
    {
        for( int i = 0; i < renderObjectCount; ++i )
        {
            RenderObject* renderObject = mRenderSet->GetRenderObject(i);
            renderObject->SetCamera(camera);
        }
    }
    for( int i = 0; i < renderObjectCount; ++i )
    {
        RenderObject* renderObject = mRenderSet->GetRenderObject(i);
        renderObject->Render(technique, pass, this);
    }
}
//----------------------------------------------------------------------------
void SubRenderer::SetTimer(GPUTimer* timer)
{
    mTimer = timer;
}
//----------------------------------------------------------------------------
GPUTimer* SubRenderer::GetTimer() const
{
    return mTimer;
}
//----------------------------------------------------------------------------
double SubRenderer::GetTimeElapsed() const
{
    if( mTimer )
    {
        return mTimer->GetTimeElapsed();
    }

    return 0.0;
}
//----------------------------------------------------------------------------
void SubRenderer::ApplyPipelineStateBlock(PipelineStateBlock* psb)
{
    if( psb->Flag & PB_Rasterizer )
    {
        if( psb->Rasterizer.Flag & RB_Viewport )
        {
            glViewport(psb->Rasterizer.Viewport.X,
                psb->Rasterizer.Viewport.Y, psb->Rasterizer.Viewport.Width,
                psb->Rasterizer.Viewport.Height);
        }
    }
    if( psb->Flag & PB_OutputMerger )
    {
        if( psb->OutputMerger.Flag & OMB_Clear )
        {
            glClear(psb->OutputMerger.ClearMask);
        }
    }
}
//----------------------------------------------------------------------------