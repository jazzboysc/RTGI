#include "RSMRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMRenderer::RSMRenderer(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
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
    TextureFormat format)
{
    AddFrameBufferTarget(RTGI_RSMRenderer_RSMPosition_Name, width, height, 
        depth, TT_Texture2DArray, format);
    AddFrameBufferTarget(RTGI_RSMRenderer_RSMNormal_Name, width, height, depth,
        TT_Texture2DArray, format);
    AddFrameBufferTarget(RTGI_RSMRenderer_RSMFlux_Name, width, height, depth,
        TT_Texture2DArray, format);
    CreateFrameBuffer(width, height, depth, TT_Texture2DArray);
}
//----------------------------------------------------------------------------
void RSMRenderer::Render(int technique, int pass, Camera* camera)
{
    SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
}
//----------------------------------------------------------------------------
