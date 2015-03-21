#ifndef RTGI_ShadowMapRenderer_H
#define RTGI_ShadowMapRenderer_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

#define RTGI_ShadowMapRenderer_ShadowMap_Name "ShadowMap"

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class ShadowMapRenderer : public SubRenderer
{
public:
    ShadowMapRenderer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~ShadowMapRenderer();

    void CreateShadowMap(int width, int height, BufferFormat format);
    void Render(int technique, int pass, Camera* camera);

protected:
    PipelineStateBlockPtr mPSB;
};

typedef RefPointer<ShadowMapRenderer> ShadowMapRendererPtr;

}

#endif