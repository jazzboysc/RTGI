//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "RenderObject.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RenderObject::RenderObject(Material* material)
	:
	mVAO(0),
	mVBO(0)
{
	assert( material );
	mMaterial = material;

	// Material object cannot be shared between render objects.
	assert( !material->mRenderObject );
	mMaterial->mRenderObject = this;
}
//----------------------------------------------------------------------------
RenderObject::~RenderObject()
{
	delete mMaterial;
	mMaterial = 0;
}
//----------------------------------------------------------------------------