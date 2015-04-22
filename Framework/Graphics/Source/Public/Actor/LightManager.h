//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_LightManager_H
#define RTGI_LightManager_H

#include "RefObject.h"
#include "GPUDevice.h"
#include "Light.h"
#include "UniformBuffer.h"
#include "SubRenderer.h"

namespace RTGI
{

struct LightBufferHead
{
    unsigned int PointLightCount;
    unsigned int SpotLightCount;
    unsigned int Reserved1;
    unsigned int Reserved2;
};

struct LightProjectorDesc
{
    float UpFovDegrees;
    float AspectRatio;
    float NearPlane;
    float FarPlane;
    glm::vec3 Location;
    glm::vec3 LookAt;
    glm::vec3 Up;
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/07/2015
//----------------------------------------------------------------------------
class LightManager : public RefObject
{
public:
    LightManager(GPUDevice* device);
    ~LightManager();

    void CreatePointLight(LightProjectorDesc* desc, Camera* lightMeshCamera);

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

    void RenderLightMesh(int technique, int pass, SubRenderer* subRenderer = 0);

    enum { MAX_LIGHT_COUNT = 256 };

private:
    GPUDevice* mDevice;
    MaterialTemplatePtr mMtLightMesh;
    std::vector<LightPtr> mPointLights;
    std::vector<LightPtr> mSpotLights;
    unsigned int mLightBufferBindingPoint;
    UniformBufferPtr mLightBuffer;
    unsigned char* mLightBufferCache;
};

typedef RefPointer<LightManager> LightManagerPtr;

}

#endif
