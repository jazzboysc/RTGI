#include "VPLRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VPLRenderer::VPLRenderer(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
{
}
//----------------------------------------------------------------------------
VPLRenderer::~VPLRenderer()
{
    mGBuffer = 0;
}
//----------------------------------------------------------------------------
void VPLRenderer::SetGBufferRenderer(GBufferRenderer* gbuffer)
{
    mGBuffer = gbuffer;

    RendererInputDataView view;
    view.Type = RDT_Texture;
    view.Sampler.MinFilter = FT_Nearest;
    view.Sampler.MagFilter = FT_Nearest;
    view.Sampler.WrapS = WT_Clamp;
    view.Sampler.WrapT = WT_Clamp;

    ClearInputDependency();

    view.BindingSlot = 0;
    AddInputDependency(gbuffer, "Position", &view);

    view.BindingSlot = 1;
    AddInputDependency(gbuffer, "Normal", &view);

    view.BindingSlot = 2;
    AddInputDependency(gbuffer, "Albedo", &view);
}
//----------------------------------------------------------------------------
GBufferRenderer* VPLRenderer::GetGBufferRenderer() const
{
    return mGBuffer;
}
//----------------------------------------------------------------------------
void VPLRenderer::CreateIndirectLightingBuffer(int width, int height,
    Texture::TextureFormat format)
{
    AddFrameBufferTarget("IndirectLighting", width, height, format);
    CreateFrameBuffer(width, height);
}
//----------------------------------------------------------------------------
void VPLRenderer::Render(int technique, int pass, Camera* camera)
{
    SubRenderer::Render(technique, pass, SRO_FrameBuffer, camera);
}
//----------------------------------------------------------------------------