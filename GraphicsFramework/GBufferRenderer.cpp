//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "GBufferRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GBufferRenderer::GBufferRenderer(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
{
}
//----------------------------------------------------------------------------
GBufferRenderer::~GBufferRenderer()
{
}
//----------------------------------------------------------------------------
void GBufferRenderer::CreateGBuffer(int width, int height, 
    Texture::TextureFormat format)
{
    AddFrameBufferTarget("Position", width, height, format);
    AddFrameBufferTarget("Normal", width, height, format);
    AddFrameBufferTarget("Albedo", width, height, format);
    CreateFrameBuffer(width, height);
}
//----------------------------------------------------------------------------
void GBufferRenderer::Render(int technique, int pass, Camera* camera)
{
    SubRenderer::Render(technique, pass, SRO_FrameBuffer, camera);
}
//----------------------------------------------------------------------------