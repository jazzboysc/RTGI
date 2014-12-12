//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_GBufferRenderer_H
#define RTGI_GBufferRenderer_H


#include "SubRenderer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class GBufferRenderer : public SubRenderer
{
public:
    GBufferRenderer(RenderSet* renderSet = 0);
    virtual ~GBufferRenderer();

    void CreateGBuffer(int width, int height, Texture::TextureFormat format);
    void Render(int technique, int pass, Camera* camera);

protected:
    PipelineStateBlockPtr mPSB;
};

typedef RefPointer<GBufferRenderer> GBufferRendererPtr;

}

#endif