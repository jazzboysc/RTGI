//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RenderObject_H
#define RTGI_RenderObject_H

#include "RenderCache.h"
#include "Material.h"
#include "Camera.h"
#include "SpatialInfo.h"
#include "Primitive.h"
#include "Pass.h"
#include "ShaderUniform.h"

namespace RTGI
{

class SubRenderer;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class RenderObject : public RefObject
{
public:
	RenderObject(Material* material);
	virtual ~RenderObject();

    void SetCamera(Camera* camera);
    Camera* GetCamera() const;

    // Spatial information delegate functions.
    inline void SetSpatialInfo(SpatialInfo* spatialInfo);
    inline SpatialInfo* GetSpatialInfo();
    inline void SetWorldTransform(const glm::mat4& worldTrans);
    inline glm::mat4 GetWorldTransform() const;
    inline void SetWorldTranslation(const glm::vec3& translation);
    inline glm::vec3 GetWorldTranslation() const;
    inline void SetWorldScale(const glm::vec3& scale);
    inline glm::vec3 GetWorldScale() const;

    // Render cache delegate functions.
    inline void SetRenderCache(RenderCache* renderCache);
    inline RenderCache* GetRenderCache();
    inline void UpdateRenderCache();

	virtual void Render(int technique, int pass, 
        SubRenderer* subRenderer = 0) = 0;
	virtual void OnUpdateShaderConstants(int technique, int pass) = 0;
    virtual void OnRender(Pass* pass, PassInfo* passInfo){};
	virtual void OnEnableBuffers(){};
	virtual void OnDisableBuffers(){};

protected:
    Primitive* mPrimitive;
	Material* mMaterial;
    Camera* mCamera;
    SpatialInfoPtr mSpatialInfo;
    RenderCachePtr mRenderCache;
};

typedef RefPointer<RenderObject> RenderObjectPtr;

#include "RenderObject.inl"

}

#endif