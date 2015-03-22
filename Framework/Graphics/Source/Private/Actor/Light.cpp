//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Light.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Light::Light()
	:
	Color(1.0f, 1.0f, 1.0f),
    mProjector(0)
{
}
//----------------------------------------------------------------------------
Light::~Light()
{
    delete mProjector;
    mLightMesh = 0;
}
//----------------------------------------------------------------------------
void Light::SetProjector(Camera* projector)
{
    mProjector = projector;
}
//----------------------------------------------------------------------------
Camera* Light::GetProjector() const
{
    return mProjector;
}
//----------------------------------------------------------------------------
void Light::SetLocation(const glm::vec3& location)
{
    if( mProjector )
    {
        mProjector->SetLocation(location);
    }
}
//----------------------------------------------------------------------------
glm::vec3 Light::GetLocation() const
{
    assert(mProjector);
    return mProjector->GetLocation();
}
//----------------------------------------------------------------------------
void Light::SetLightMesh(TriangleMesh* lightMesh)
{
    mLightMesh = lightMesh;
}
//----------------------------------------------------------------------------
TriangleMesh* Light::GetLightMesh() const
{
    return mLightMesh;
}
//----------------------------------------------------------------------------