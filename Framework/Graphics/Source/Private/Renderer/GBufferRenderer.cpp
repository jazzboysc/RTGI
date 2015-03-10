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
void GBufferRenderer::CreateGBuffer(GBufferDesc* desc)
{
    assert(desc);

    AddFrameBufferTarget(RTGI_GBuffer_Position_Name, desc->Width, 
        desc->Height, 0, TT_Texture2D, desc->PositionFormat, 
        desc->PositionMipmap);
    AddFrameBufferTarget(RTGI_GBuffer_Normal_Name, desc->Width, desc->Height, 
        0, TT_Texture2D, desc->NormalFormat, desc->NormalMipmap);
    AddFrameBufferTarget(RTGI_GBuffer_Albedo_Name, desc->Width, desc->Height, 
        0, TT_Texture2D, desc->AlbedoFormat, desc->AlbedoMipmap);
    CreateFrameBuffer(desc->Width, desc->Height, 0, TT_Texture2D);
}
//----------------------------------------------------------------------------
void GBufferRenderer::Render(int technique, int pass, Camera* camera)
{
    SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
}
//----------------------------------------------------------------------------