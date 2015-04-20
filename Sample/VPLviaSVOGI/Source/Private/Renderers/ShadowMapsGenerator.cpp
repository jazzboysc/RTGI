#include "ShadowMapsGenerator.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ShadowMapsGenerator::ShadowMapsGenerator(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet),
    mShadowMapInfoBufferBindingPoint(0)
{
    mPSB = new PipelineStateBlock();
    mPSB->Flag |= PB_OutputMerger;
    mPSB->OutputMerger.Flag |= OMB_Clear;
    mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

    mShadowMapCount = 0;
}
//----------------------------------------------------------------------------
ShadowMapsGenerator::~ShadowMapsGenerator()
{
    mPSB = 0;
    mLightManager = 0;
    mShadowMapInfoBuffer = 0;
}
//----------------------------------------------------------------------------
void ShadowMapsGenerator::Initialize(int width, int height,
    BufferFormat format, LightManager* lightManager)
{
    RTGI_ASSERT(lightManager);
    mLightManager = lightManager;

    // Compute how many shadow maps are needed.
    int pointLightCount = mLightManager->GetPointLightCount();
    int spotLightCount = mLightManager->GetSpotLightCount();
    mShadowMapCount = pointLightCount + spotLightCount;

    // Create shadow map array.
    AddFrameBufferTarget(RTGI_ShadowMapRenderer_ShadowMaps_Name, width, height,
        mShadowMapCount, TT_Texture2DArray, format);
    CreateFrameBuffer(width, height, mShadowMapCount, TT_Texture2DArray);

    // Create shadow map info uniform buffer.
    mShadowMapInfoBuffer = new UniformBuffer();
    size_t bufferSize = sizeof(ShadowMapInfo);
    mShadowMapInfoBuffer->ReserveMutableDeviceResource(mDevice, bufferSize,
        BU_Dynamic_Copy);
}
//----------------------------------------------------------------------------
void ShadowMapsGenerator::Render(int technique, int pass)
{
    PreRender(SRO_FrameBuffer, mPSB);

    int i = 0;
    for( ; i < mLightManager->GetPointLightCount(); ++i )
    {
        // Update shadow map info uniform buffer. Let the shader program know
        // which light is being used for shadow map generation.
        mShadowMapInfoBuffer->UpdateSubData(mShadowMapInfoBufferBindingPoint, 
            0, sizeof(unsigned int), (void*)&i);

        // We don't need to pass in a camera object since we are using a light
        // uniform buffer updated by light manager.
        SubRenderer::OnRender(technique, pass, 0);
    }
    for( int j = 0; j < mLightManager->GetSpotLightCount(); ++j )
    {
        int curLightIndex = i + j;
        mShadowMapInfoBuffer->UpdateSubData(mShadowMapInfoBufferBindingPoint,
            0, sizeof(unsigned int), (void*)&curLightIndex);

        SubRenderer::OnRender(technique, pass, 0);
    }

    PostRender(SRO_FrameBuffer, mPSB);
}
//----------------------------------------------------------------------------