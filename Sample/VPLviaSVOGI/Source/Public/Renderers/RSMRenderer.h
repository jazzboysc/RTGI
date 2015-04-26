#ifndef RTGI_RSMRenderer_H
#define RTGI_RSMRenderer_H

#include "GraphicsFrameworkHeader.h"
#include "LightManager.h"

namespace RTGI
{

#define RTGI_RSMRenderer_RSMPosition_Name "RSMPosition"
#define RTGI_RSMRenderer_RSMNormal_Name "RSMNormal"
#define RTGI_RSMRenderer_RSMFlux_Name "RSMFlux"

struct RSMInfo
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
class RSMRenderer : public SubRenderer
{
public:
    RSMRenderer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~RSMRenderer();

    void Initialize(int width, int height, BufferFormat format,
        LightManager* lightManager);
    void Render(int technique, int pass);

    inline int GetRSMCount() const { return mRSMCount; };
    inline void SetRSMInfoBufferBindingPoint(unsigned int bindingPoint)
    {
        mRSMInfoBufferBindingPoint = bindingPoint;
    };
    inline unsigned int GetRSMInfoBufferBindingPoint() const
    {
        return mRSMInfoBufferBindingPoint;
    };

protected:
    PipelineStateBlockPtr mPSB;
    LightManagerPtr mLightManager;
    int mRSMCount;
    UniformBufferPtr mRSMInfoBuffer;
    unsigned int mRSMInfoBufferBindingPoint;
};

typedef RefPointer<RSMRenderer> RSMRendererPtr;

}

#endif