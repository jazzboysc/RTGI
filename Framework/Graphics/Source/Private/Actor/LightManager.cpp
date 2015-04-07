//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "LightManager.h"

using namespace RTGI;

//----------------------------------------------------------------------------
LightManager::LightManager()
    :
    mLightBufferBindingPoint(0)
{
    memset(mLightBufferCache, 0, sizeof(SceneLight)*MAX_LIGHT_COUNT);
}
//----------------------------------------------------------------------------
LightManager::~LightManager()
{
    for( int i = 0; i < (int)mLights.size(); ++i )
    {
        mLights[i] = 0;
    }
    mLightBuffer = 0;
}
//----------------------------------------------------------------------------
void LightManager::AddLight(Light* light)
{
    RTGI_ASSERT(mLights.size() < MAX_LIGHT_COUNT);
    mLights.push_back(light);
}
//----------------------------------------------------------------------------
Light* LightManager::GetLight(int i) const
{
    RTGI_ASSERT(i >= 0 && i < (int)mLights.size());
    return mLights[i];
}
//----------------------------------------------------------------------------
void LightManager::CreateLightBuffer(GPUDevice* device)
{
    if( mLights.size() > 0 )
    {
        mLightBuffer = new UniformBuffer();
        size_t bufferSize = sizeof(SceneLight) * mLights.size();
        mLightBuffer->ReserveMutableDeviceResource(device, bufferSize,
            BU_Dynamic_Draw);

        UpdateLightBuffer();
    }
}
//----------------------------------------------------------------------------
void LightManager::UpdateLightBuffer()
{
    for( int i = 0; i < (int)mLights.size(); ++i )
    {
        mLights[i]->OnUpdateLightBufferCache(&mLightBufferCache[i]);
    }

    size_t bufferSize = sizeof(SceneLight) * mLights.size();
    mLightBuffer->UpdateSubData(mLightBufferBindingPoint, 0, bufferSize,
        (void*)mLightBufferCache);
}
//----------------------------------------------------------------------------
void LightManager::SetLightBufferBindingPoint(unsigned int bindingPoint)
{
    mLightBufferBindingPoint = bindingPoint;
}
//----------------------------------------------------------------------------
unsigned int LightManager::GetLightBufferBindingPoint() const
{
    return mLightBufferBindingPoint;
}
//----------------------------------------------------------------------------