#include "IndirectLightingRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
IndirectLightingRenderer::IndirectLightingRenderer(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
{
}
//----------------------------------------------------------------------------
IndirectLightingRenderer::~IndirectLightingRenderer()
{
    mGBuffer = 0;
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::SetGBufferRenderer(GBufferRenderer* gbuffer)
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
GBufferRenderer* IndirectLightingRenderer::GetGBufferRenderer() const
{
    return mGBuffer;
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::CreateIndirectLightingBuffer(int width, 
    int height, Texture::TextureFormat format)
{
    AddFrameBufferTarget("IndirectLighting", width, height, 0, 
        Texture::TT_Texture2D, format);
    CreateFrameBuffer(width, height, 0, Texture::TT_Texture2D);
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::Render(int technique, int pass, Camera* camera)
{
    SubRenderer::Render(technique, pass, SRO_FrameBuffer, 0, camera);
}
//----------------------------------------------------------------------------