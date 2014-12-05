#ifndef RTGI_DirectLightingRenderer_H
#define RTGI_DirectLightingRenderer_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class DirectLightingRenderer : public SubRenderer
{
public:
    DirectLightingRenderer(RenderSet* renderSet = 0);
    virtual ~DirectLightingRenderer();
};

}

#endif