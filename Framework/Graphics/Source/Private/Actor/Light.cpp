//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Light.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Light::Light()
	:
    Intensity(50.0f, 50.0f, 50.0f),
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
    mLocation = mProjector->GetLocation();
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
    mLocation = location;
}
//----------------------------------------------------------------------------
glm::vec3 Light::GetLocation() const
{
    return mLocation;
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
void Light::RenderLightMesh(int technique, int pass, SubRenderer* subRenderer)
{
    RTGI_ASSERT(mLightMesh);
    mLightMesh->Render(technique, pass, subRenderer);
}
//----------------------------------------------------------------------------
void Light::OnUpdateLightBufferCache(SceneLight* cache)
{
    RTGI_ASSERT(cache);
    if( cache )
    {
        cache->Intensity.x = Intensity.x;
        cache->Intensity.y = Intensity.y;
        cache->Intensity.z = Intensity.z;

        cache->WorldPositionAndType.x = mLocation.x;
        cache->WorldPositionAndType.y = mLocation.y;
        cache->WorldPositionAndType.z = mLocation.z;

        RTGI_ASSERT(mProjector);
        if( mProjector )
        {
            glm::mat4 lightView = mProjector->GetViewTransform();
            cache->LightProjectorView = lightView;

            glm::mat4 lightProj = mProjector->GetProjectionTransform();
            cache->LightProjectorProj = lightProj;

            mProjector->GetNearFarPlane(
                (float*)&cache->LightProjectorNearFar);
        }
    }
}
//----------------------------------------------------------------------------