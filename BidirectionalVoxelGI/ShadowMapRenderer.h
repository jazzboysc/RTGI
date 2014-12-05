#ifndef RTGI_ShadowMapRenderer_H
#define RTGI_ShadowMapRenderer_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class ShadowMapRenderer : public SubRenderer
{
public:
    ShadowMapRenderer(RenderSet* renderSet = 0);
    virtual ~ShadowMapRenderer();
};

}

#endif