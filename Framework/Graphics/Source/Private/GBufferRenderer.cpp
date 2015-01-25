//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "GBufferRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GBufferRenderer::GBufferRenderer(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
{
    mPSB = new PipelineStateBlock();
    mPSB->Flag |= PB_OutputMerger;
    mPSB->OutputMerger.Flag |= OMB_Clear;
    mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
}
//----------------------------------------------------------------------------
GBufferRenderer::~GBufferRenderer()
{
    mPSB = 0;
}
//----------------------------------------------------------------------------
void GBufferRenderer::CreateGBuffer(int width, int height, 
    BufferFormat format)
{
    AddFrameBufferTarget(RTGI_GBuffer_Position_Name, width, height, 0, 
        TT_Texture2D, format);
    AddFrameBufferTarget(RTGI_GBuffer_Normal_Name, width, height, 0, 
        TT_Texture2D, format);
    AddFrameBufferTarget(RTGI_GBuffer_Albedo_Name, width, height, 0, 
        TT_Texture2D, format);
    CreateFrameBuffer(width, height, 0, TT_Texture2D);
}
//----------------------------------------------------------------------------
void GBufferRenderer::Render(int technique, int pass, Camera* camera)
{
    SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
}
//----------------------------------------------------------------------------