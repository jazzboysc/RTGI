//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Light.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Light::Light()
	:
	Color(1.0f, 1.0f, 1.0f),
	mLocation(0.0f, 0.0f, 0.0f),
    mProjector(0)
{
}
//----------------------------------------------------------------------------
Light::~Light()
{
}
//----------------------------------------------------------------------------
void Light::SetProjector(Camera* projector)
{
    mProjector = projector;
    if( mProjector )
    {
        mLocation = mProjector->GetLocation();
    }
}
//----------------------------------------------------------------------------
Camera* Light::GetProjector() const
{
    return mProjector;
}
//----------------------------------------------------------------------------
void Light::SetLocation(const vec3& location)
{
    mLocation = location;
    if( mProjector )
    {
        mProjector->SetLocation(location);
    }
}
//----------------------------------------------------------------------------
vec3 Light::GetLocation() const
{
    return mLocation;
}
//----------------------------------------------------------------------------