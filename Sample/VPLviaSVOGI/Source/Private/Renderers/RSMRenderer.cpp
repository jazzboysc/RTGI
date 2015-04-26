#include "RSMRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMRenderer::RSMRenderer(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet),
    mRSMInfoBufferBindingPoint(0)
{
    mPSB = new PipelineStateBlock();
    mPSB->Flag |= PB_OutputMerger;
    mPSB->OutputMerger.Flag |= OMB_Clear;
    mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;

    mRSMCount = 0;
}
//----------------------------------------------------------------------------
RSMRenderer::~RSMRenderer()
{
    mPSB = 0;
    mLightManager = 0;
    mRSMInfoBuffer = 0;
}
//----------------------------------------------------------------------------
void RSMRenderer::Initialize(int width, int height, BufferFormat format,
    LightManager* lightManager)
{
    RTGI_ASSERT(lightManager);
    mLightManager = lightManager;

    // Compute how many RSMs are needed.
    int pointLightCount = mLightManager->GetPointLightCount();
    int spotLightCount = mLightManager->GetSpotLightCount();
    mRSMCount = pointLightCount*6 + spotLightCount;

    // Create RSM texture array.
    AddFrameBufferTarget(RTGI_RSMRenderer_RSMPosition_Name, width, height, 
        mRSMCount, TT_Texture2DArray, format);
    AddFrameBufferTarget(RTGI_RSMRenderer_RSMNormal_Name, width, height, 
        mRSMCount, TT_Texture2DArray, format);
    AddFrameBufferTarget(RTGI_RSMRenderer_RSMFlux_Name, width, height, 
        mRSMCount, TT_Texture2DArray, format);
    CreateFrameBuffer(width, height, mRSMCount, TT_Texture2DArray);

    // Create RSM info uniform buffer.
    mRSMInfoBuffer = new UniformBuffer();
    size_t bufferSize = sizeof(RSMInfo);
    mRSMInfoBuffer->ReserveMutableDeviceResource(mDevice, bufferSize,
        BU_Dynamic_Copy);
}
//----------------------------------------------------------------------------
void RSMRenderer::Render(int technique, int pass)
{
    PreRender(SRO_FrameBuffer, mPSB);

    int i = 0;
    for( ; i < mLightManager->GetPointLightCount(); ++i )
    {
        // Update RSM info uniform buffer. Let the shader program know
        // which light is being used for RSM generation.
        mRSMInfoBuffer->UpdateSubData(mRSMInfoBufferBindingPoint, 0, 
            sizeof(unsigned int), (void*)&i);

        // We don't need to pass in a camera object since we are using a light
        // uniform buffer updated by light manager.
        SubRenderer::OnRender(technique, pass, 0);
    }
    for( int j = 0; j < mLightManager->GetSpotLightCount(); ++j )
    {
        int curLightIndex = i + j;
        mRSMInfoBuffer->UpdateSubData(mRSMInfoBufferBindingPoint, 0, 
            sizeof(unsigned int), (void*)&curLightIndex);

        // Assume spot light RSM pass = point light RSM pass + 1.
        SubRenderer::OnRender(technique, pass + 1, 0);
    }

    PostRender(SRO_FrameBuffer, mPSB);
}
//----------------------------------------------------------------------------
