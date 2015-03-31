//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "RenderObject.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RenderObject::RenderObject(Material* material)
{
    mPrimitive = new Primitive();
    mMaterial = material;

    // Material object cannot be shared between render objects.
    if( mMaterial )
    {
        RTGI_ASSERT(!material->mRenderObject);
        mMaterial->mRenderObject = this;
    }
}
//----------------------------------------------------------------------------
RenderObject::~RenderObject()
{
    mPrimitive = 0;
	mMaterial = 0;
    mSpatialInfo = 0;
    mRenderCache = 0;
}
//----------------------------------------------------------------------------
void RenderObject::SetCamera(Camera* camera)
{
    mCamera = camera;
}
//----------------------------------------------------------------------------
Camera* RenderObject::GetCamera() const
{
    return mCamera;
}
//----------------------------------------------------------------------------
int RenderObject::GetVoxelizerRasterDimension(Voxelizer*)
{
    RTGI_ASSERT(false);
    return 0;
}
//----------------------------------------------------------------------------
Primitive* RenderObject::GetPrimitive() const
{
    return mPrimitive;
}
//----------------------------------------------------------------------------
Material* RenderObject::GetMaterial() const
{
    return mMaterial;
}
//----------------------------------------------------------------------------
void RenderObject::SetWorldTransform(const glm::mat4& worldTrans)
{
    RTGI_ASSERT(mSpatialInfo);
    mSpatialInfo->SetWorldTransform(worldTrans);
}
//----------------------------------------------------------------------------
glm::mat4 RenderObject::GetWorldTransform() const
{
    RTGI_ASSERT(mSpatialInfo);
    return mSpatialInfo->GetWorldTransform();
}
//----------------------------------------------------------------------------
void RenderObject::SetWorldTranslation(const glm::vec3& translation)
{
    RTGI_ASSERT(mSpatialInfo);
    mSpatialInfo->SetWorldTranslation(translation);
}
//----------------------------------------------------------------------------
glm::vec3 RenderObject::GetWorldTranslation() const
{
    RTGI_ASSERT(mSpatialInfo);
    return mSpatialInfo->GetWorldTranslation();
}
//----------------------------------------------------------------------------
void RenderObject::SetWorldScale(const glm::vec3& scale)
{
    RTGI_ASSERT(mSpatialInfo);
    mSpatialInfo->SetWorldScale(scale);
}
//----------------------------------------------------------------------------
glm::vec3 RenderObject::GetWorldScale() const
{
    RTGI_ASSERT(mSpatialInfo);
    return mSpatialInfo->GetWorldScale();
}
//----------------------------------------------------------------------------
void RenderObject::SetRenderCache(RenderCache* renderCache)
{
    mRenderCache = renderCache;
}
//----------------------------------------------------------------------------
RenderCache* RenderObject::GetRenderCache()
{
    return mRenderCache;
}
//----------------------------------------------------------------------------
void RenderObject::UpdateRenderCache()
{
    RTGI_ASSERT(mRenderCache && mSpatialInfo);
    mRenderCache->Update(mSpatialInfo);
}
//----------------------------------------------------------------------------