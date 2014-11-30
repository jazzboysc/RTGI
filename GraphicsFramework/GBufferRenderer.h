//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_GBufferRenderer_H
#define RTGI_GBufferRenderer_H

#include "FrameworkCommon.h"
#include "SubRenderer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2013
//----------------------------------------------------------------------------
class GBufferRenderer : public SubRenderer
{
public:
    GBufferRenderer(SceneManager* sceneManager);
    virtual ~GBufferRenderer();

    void CreateGBuffer(int width, int height, Texture::TextureFormat format);
};

typedef RefPointer<GBufferRenderer> GBufferRendererPtr;

}

#endif