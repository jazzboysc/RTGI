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

    assert( material );
    mMaterial = material;

    // Material object cannot be shared between render objects.
    assert( !material->mRenderObject );
    mMaterial->mRenderObject = this;
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