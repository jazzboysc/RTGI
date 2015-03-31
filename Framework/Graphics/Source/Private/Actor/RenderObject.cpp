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
    delete mPrimitive;
	delete mMaterial;
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