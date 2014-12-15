#include "ShadowMapRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ShadowMapRenderer::ShadowMapRenderer(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
{
    mPSB = new PipelineStateBlock();
    mPSB->Flag |= PB_OutputMerger;
    mPSB->OutputMerger.Flag |= OMB_Clear;
    mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
}
//----------------------------------------------------------------------------
ShadowMapRenderer::~ShadowMapRenderer()
{
    mPSB = 0;
}
//----------------------------------------------------------------------------
void ShadowMapRenderer::CreateShadowMap(int width, int height,
    Texture::TextureFormat format)
{
    AddFrameBufferTarget("ShadowMap", width, height, 0, Texture::TT_Texture2D, 
        format);
    CreateFrameBuffer(width, height, 0, Texture::TT_Texture2D);
}
//----------------------------------------------------------------------------
void ShadowMapRenderer::Render(int technique, int pass, Camera* camera)
{
    SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
}
//----------------------------------------------------------------------------