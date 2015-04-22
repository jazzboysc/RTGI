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

struct PointLightDesc
{
    glm::vec3 Intensity;
};

struct SpotLightDesc
{
    glm::vec3 Intensity;
    float CosCutoff;
    float InnerCosCutoff;
    float SpotExponent;
    float ConstantAttenuation;
    float QuadraticAttenuation;
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

    // Create a point light and add it to light manager.
    Light* CreatePointLight(LightProjectorDesc* projectorDesc, 
        Camera* lightMeshCamera, PointLightDesc* pointLightDesc);

    // Create a spot light and add it to light manager.
    Light* CreateSpotLight(LightProjectorDesc* projectorDesc,
        Camera* lightMeshCamera, SpotLightDesc* spotLightDesc);

    // Light access.
    void AddPointLight(Light* light);
    Light* GetPointLight(int i) const;
    int GetPointLightCount() const;
    void AddSpotLight(Light* light);
    Light* GetSpotLight(int i) const;
    int GetSpotLightCount() const;

    // Light buffer is a uniform buffer managed by light manager. It stores
    // lights' information that can be accessed by lighting shaders.
    void CreateLightBuffer(GPUDevice* device);
    void UpdateLightBuffer();

    void SetLightBufferBindingPoint(unsigned int bindingPoint);
    unsigned int GetLightBufferBindingPoint() const;

    // Show light meshes as scene objects.
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
