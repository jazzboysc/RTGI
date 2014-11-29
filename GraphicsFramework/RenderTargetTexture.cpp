//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "RenderTargetTexture.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RenderTargetTexture::RenderTargetTexture(const std::string& name, 
    Texture2D* texture)
{
    Name = name;
    Texture = texture;
}
//----------------------------------------------------------------------------
RenderTargetTexture::~RenderTargetTexture()
{
    Texture = 0;
}
//----------------------------------------------------------------------------