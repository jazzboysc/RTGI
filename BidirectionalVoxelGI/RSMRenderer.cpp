#include "RSMRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMRenderer::RSMRenderer(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
{
    mPSB = new PipelineStateBlock();
    mPSB->Flag |= PB_OutputMerger;
    mPSB->OutputMerger.Flag |= OMB_Clear;
    mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
}
//----------------------------------------------------------------------------
RSMRenderer::~RSMRenderer()
{
    mPSB = 0;
}
//----------------------------------------------------------------------------
void RSMRenderer::CreateRSM(int width, int height, int depth,
    Texture::TextureFormat format)
{
    AddFrameBufferTarget("RSMPosition", width, height, depth, 
        Texture::TT_Texture2DArray, format);
    AddFrameBufferTarget("RSMNormal", width, height, depth,
        Texture::TT_Texture2DArray, format);
    AddFrameBufferTarget("RSMFlux", width, height, depth,
        Texture::TT_Texture2DArray, format);
    CreateFrameBuffer(width, height, depth, Texture::TT_Texture2DArray);
}
//----------------------------------------------------------------------------
void RSMRenderer::Render(int technique, int pass, Camera* camera)
{
    SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
}
//----------------------------------------------------------------------------
