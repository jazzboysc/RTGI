#ifndef RTGI_ShadowMapsGenerator_H
#define RTGI_ShadowMapsGenerator_H

#include "SubRenderer.h"
#include "LightManager.h"

namespace RTGI
{

#define RTGI_ShadowMapRenderer_ShadowMaps_Name "ShadowMaps"

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class ShadowMapsGenerator : public SubRenderer
{
public:
    ShadowMapsGenerator(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~ShadowMapsGenerator();

    void Initialize(int width, int height, BufferFormat format, 
        LightManager* lightManager);
    void Render(int technique, int pass, Camera* camera);

    inline int GetShadowMapCount() const { return mShadowMapCount; };

protected:
    PipelineStateBlockPtr mPSB;
    LightManagerPtr mLightManager;
    int mShadowMapCount;
};

typedef RefPointer<ShadowMapsGenerator> ShadowMapsGeneratorPtr;

}

#endif