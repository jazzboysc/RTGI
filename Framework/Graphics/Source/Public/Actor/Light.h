//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Light_H
#define RTGI_Light_H

#include "RefObject.h"
#include "Camera.h"
#include "TriangleMesh.h"

namespace RTGI
{

// CPU memory footage of scene light used by shaders.
struct SceneLight
{
    // type 1: point light, type 2: spot light
    glm::vec4 WorldPositionAndType;
    glm::vec4 Intensity;

    // SpotDirection, CosCutoff
    glm::vec4 Params1;
    // SpotExponent, ConstantAttenuation, QuadraticAttenuation, 
    // SpotInnerCosCutoff
    glm::vec4 Params2;

    glm::mat4 LightProjectorView;
    glm::mat4 LightProjectorProj;
    glm::vec4 LightProjectorNearFar;
};

enum LightType
{
    LT_Point = 1,
    LT_Spot  = 2
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/14/2013
//----------------------------------------------------------------------------
class Light : public RefObject
{
public:
    Light(LightType type = LT_Point);
    virtual ~Light();

	glm::vec3 Intensity;
    
    void SetProjector(Camera* projector);
    Camera* GetProjector() const;
    void SetLocation(const glm::vec3& location);
    glm::vec3 GetLocation() const;

    void SetLightMesh(TriangleMesh* lightMesh);
    TriangleMesh* GetLightMesh() const;

    void RenderLightMesh(int technique, int pass, 
        SubRenderer* subRenderer = 0);

    // Called by light manager to update shader uniform buffer for this light.
    virtual void OnUpdateLightBufferCache(SceneLight* cache);
   
protected:
    LightType mLightType;
    glm::vec3 mLocation;
    Camera* mProjector;
    TriangleMeshPtr mLightMesh;

    glm::vec4 mParams1;
    glm::vec4 mParams2;
};

typedef RefPointer<Light> LightPtr;

}

#endif
