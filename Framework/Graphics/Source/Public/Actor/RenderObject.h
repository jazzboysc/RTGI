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
class Voxelizer;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class RenderObject : public RefObject
{
public:
	RenderObject(Material* material);
	virtual ~RenderObject();

    virtual void SetCamera(Camera* camera);
    virtual Camera* GetCamera() const;

    virtual int GetVoxelizerRasterDimension(Voxelizer* voxelizer);

    // Primitive access.
    virtual Primitive* GetPrimitive() const;

    // Material access.
    virtual Material* GetMaterial() const;

    // Spatial information delegate functions.
    inline void SetSpatialInfo(SpatialInfo* spatialInfo);
    inline SpatialInfo* GetSpatialInfo();
    virtual void SetWorldTransform(const glm::mat4& worldTrans);
    virtual glm::mat4 GetWorldTransform() const;
    virtual void SetWorldTranslation(const glm::vec3& translation);
    virtual glm::vec3 GetWorldTranslation() const;
    virtual void SetWorldScale(const glm::vec3& scale);
    virtual glm::vec3 GetWorldScale() const;

    // Render cache delegate functions.
    virtual void SetRenderCache(RenderCache* renderCache);
    virtual RenderCache* GetRenderCache();
    virtual void UpdateRenderCache();

	virtual void Render(int technique, int pass, 
        SubRenderer* subRenderer = 0) = 0;
	virtual void OnUpdateShaderConstants(int technique, int pass) = 0;
    virtual void OnRender(Pass* pass, PassInfo* passInfo){};
	virtual void OnEnableBuffers(){};
	virtual void OnDisableBuffers(){};

protected:
    PrimitivePtr mPrimitive;
	MaterialPtr mMaterial;
    Camera* mCamera;
    SpatialInfoPtr mSpatialInfo;
    RenderCachePtr mRenderCache;
};

typedef RefPointer<RenderObject> RenderObjectPtr;

#include "RenderObject.inl"

}

#endif