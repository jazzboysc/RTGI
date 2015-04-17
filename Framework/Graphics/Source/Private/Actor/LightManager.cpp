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
    size_t bufferSize = sizeof(LightBufferHead) + 
        sizeof(SceneLight)*MAX_LIGHT_COUNT;
    mLightBufferCache = new unsigned char[bufferSize];
    memset(mLightBufferCache, 0, bufferSize);
}
//----------------------------------------------------------------------------
LightManager::~LightManager()
{
    for( int i = 0; i < (int)mPointLights.size(); ++i )
    {
        mPointLights[i] = 0;
    }

    for( int i = 0; i < (int)mSpotLights.size(); ++i )
    {
        mSpotLights[i] = 0;
    }

    delete[] mLightBufferCache;
    mLightBuffer = 0;
}
//----------------------------------------------------------------------------
void LightManager::AddPointLight(Light* light)
{
    RTGI_ASSERT((mPointLights.size() + mSpotLights.size()) < MAX_LIGHT_COUNT);
    mPointLights.push_back(light);
}
//----------------------------------------------------------------------------
Light* LightManager::GetPointLight(int i) const
{
    RTGI_ASSERT(i >= 0 && i < (int)mPointLights.size());
    return mPointLights[i];
}
//----------------------------------------------------------------------------
int LightManager::GetPointLightCount() const
{
    return mPointLights.size();
}
//----------------------------------------------------------------------------
void LightManager::AddSpotLight(Light* light)
{
    RTGI_ASSERT((mPointLights.size() + mSpotLights.size()) < MAX_LIGHT_COUNT);
    mSpotLights.push_back(light);
}
//----------------------------------------------------------------------------
Light* LightManager::GetSpotLight(int i) const
{
    RTGI_ASSERT(i >= 0 && i < (int)mSpotLights.size());
    return mSpotLights[i];
}
//----------------------------------------------------------------------------
int LightManager::GetSpotLightCount() const
{
    return mSpotLights.size();
}
//----------------------------------------------------------------------------
void LightManager::CreateLightBuffer(GPUDevice* device)
{
    if( mPointLights.size() > 0 || mSpotLights.size() > 0 )
    {
        mLightBuffer = new UniformBuffer();
        size_t bufferSize = sizeof(LightBufferHead) + 
            sizeof(SceneLight)*MAX_LIGHT_COUNT;
        mLightBuffer->ReserveMutableDeviceResource(device, bufferSize,
            BU_Dynamic_Draw);

        UpdateLightBuffer();
    }
}
//----------------------------------------------------------------------------
void LightManager::UpdateLightBuffer()
{
    LightBufferHead* info = (LightBufferHead*)mLightBufferCache;
    info->CurLightIndex = 0;
    info->PointLightCount = (unsigned int)mPointLights.size();
    info->SpotLightCount = (unsigned int)mSpotLights.size();

    SceneLight* curLight = (SceneLight*)(mLightBufferCache + 
        sizeof(LightBufferHead));
    for( int i = 0; i < (int)mPointLights.size(); ++i )
    {
        mPointLights[i]->OnUpdateLightBufferCache(curLight);
        curLight++;
    }
    for( int i = 0; i < (int)mSpotLights.size(); ++i )
    {
        mSpotLights[i]->OnUpdateLightBufferCache(curLight);
        curLight++;
    }

    size_t bufferSize = sizeof(LightBufferHead) + 
        sizeof(SceneLight)*MAX_LIGHT_COUNT;
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