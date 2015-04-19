#include "ShadowMapsGenerator.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ShadowMapsGenerator::ShadowMapsGenerator(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
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
}
//----------------------------------------------------------------------------
void ShadowMapsGenerator::Initialize(int width, int height,
    BufferFormat format, LightManager* lightManager)
{
    RTGI_ASSERT(lightManager);
    mLightManager = lightManager;

    int pointLightCount = mLightManager->GetPointLightCount();
    int spotLightCount = mLightManager->GetSpotLightCount();
    mShadowMapCount = pointLightCount + spotLightCount;

    AddFrameBufferTarget(RTGI_ShadowMapRenderer_ShadowMaps_Name, width, height,
        mShadowMapCount, TT_Texture2DArray, format);
    CreateFrameBuffer(width, height, mShadowMapCount, TT_Texture2DArray);
}
//----------------------------------------------------------------------------
void ShadowMapsGenerator::Render(int technique, int pass, Camera* camera)
{
    SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
}
//----------------------------------------------------------------------------