//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_LightManager_H
#define RTGI_LightManager_H

#include "RefObject.h"
#include "Light.h"
#include "UniformBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/07/2015
//----------------------------------------------------------------------------
class LightManager : public RefObject
{
public:
    LightManager();
    ~LightManager();

    void AddPointLight(Light* light);
    Light* GetPointLight(int i) const;
    int GetPointLightCount() const;
    void AddSpotLight(Light* light);
    Light* GetSpotLight(int i) const;
    int GetSpotLightCount() const;

    void CreateLightBuffer(GPUDevice* device);
    void UpdateLightBuffer();
    void SetLightBufferBindingPoint(unsigned int bindingPoint);
    unsigned int GetLightBufferBindingPoint() const;

    enum { MAX_LIGHT_COUNT = 256 };

private:
    std::vector<LightPtr> mPointLights;
    std::vector<LightPtr> mSpotLights;
    unsigned int mLightBufferBindingPoint;
    UniformBufferPtr mLightBuffer;
    SceneLight mLightBufferCache[MAX_LIGHT_COUNT];
};

typedef RefPointer<LightManager> LightManagerPtr;

}

#endif
