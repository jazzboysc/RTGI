#ifndef RTGI_ShadowMapsGenerator_H
#define RTGI_ShadowMapsGenerator_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

#define RTGI_ShadowMapRenderer_ShadowMap_Name "ShadowMap"

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class ShadowMapsGenerator : public SubRenderer
{
public:
    ShadowMapsGenerator(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~ShadowMapsGenerator();

    void CreateShadowMap(int width, int height, BufferFormat format);
    void Render(int technique, int pass, Camera* camera);

protected:
    PipelineStateBlockPtr mPSB;
};

typedef RefPointer<ShadowMapsGenerator> ShadowMapsGeneratorPtr;

}

#endif