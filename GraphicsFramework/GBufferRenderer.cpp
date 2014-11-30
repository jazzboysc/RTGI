//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "GBufferRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GBufferRenderer::GBufferRenderer(SceneManager* sceneManager)
    :
    SubRenderer(sceneManager)
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
    AddRenderTarget("Position", width, height, format);
    AddRenderTarget("Normal", width, height, format);
    AddRenderTarget("Albedo", width, height, format);
    CreateFrameBuffer(width, height);
}
//----------------------------------------------------------------------------