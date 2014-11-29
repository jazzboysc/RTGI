//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RenderTargetTexture_H
#define RTGI_RenderTargetTexture_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Texture2D.h"
#include <string>

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2013
//----------------------------------------------------------------------------
class RenderTargetTexture : public RefObject
{
public:
    RenderTargetTexture(const std::string& name, Texture2D* texture);
    virtual ~RenderTargetTexture();

    std::string Name;
    Texture2DPtr Texture;
};

typedef RefPointer<RenderTargetTexture> RenderTargetTexturePtr;

}

#endif