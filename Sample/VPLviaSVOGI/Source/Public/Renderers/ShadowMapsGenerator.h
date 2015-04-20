#ifndef RTGI_ShadowMapsGenerator_H
#define RTGI_ShadowMapsGenerator_H

#include "SubRenderer.h"
#include "LightManager.h"

namespace RTGI
{

#define RTGI_ShadowMapRenderer_ShadowMaps_Name "ShadowMaps"

struct ShadowMapInfo
{
    uint CurLightIndex;
    uint Reserved1;
    uint Reserved2;
    uint Reserved3;
};

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
    void Render(int technique, int pass);

    inline int GetShadowMapCount() const { return mShadowMapCount; };
    inline void SetShadowMapInfoBufferBindingPoint(unsigned int bindingPoint)
    {
        mShadowMapInfoBufferBindingPoint = bindingPoint;
    };
    inline unsigned int GetShadowMapInfoBufferBindingPoint() const
    {
        return mShadowMapInfoBufferBindingPoint;
    };

protected:
    PipelineStateBlockPtr mPSB;
    LightManagerPtr mLightManager;
    int mShadowMapCount;
    UniformBufferPtr mShadowMapInfoBuffer;
    unsigned int mShadowMapInfoBufferBindingPoint;
};

typedef RefPointer<ShadowMapsGenerator> ShadowMapsGeneratorPtr;

}

#endif