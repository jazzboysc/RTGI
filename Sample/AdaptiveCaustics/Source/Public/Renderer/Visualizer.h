#ifndef RTGI_Visualizer_H
#define RTGI_Visualizer_H

#include "GraphicsFrameworkHeader.h"
#include "ReceiverResourceRenderer.h"
#include "RefractorResourceRenderer.h"
#include "RefractorResourceRendererBack.h"
namespace RTGI
{
//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class VisualizerScreenQuad : public ScreenQuad
{
public:
    VisualizerScreenQuad(Material* material);
    virtual ~VisualizerScreenQuad();

    // Implement base class interface.
    virtual void OnUpdateShaderConstants(int technique, int pass);
    virtual void OnGetShaderConstants();

	int ShowMode;

    Texture2DPtr DisplayTexture;

private:
    ShaderUniform mShowModeLoc;
    ShaderUniform mTempSamplerLoc;
};

typedef RefPointer<VisualizerScreenQuad> VisualizerScreenQuadPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class Visualizer : public SubRenderer
{
public:
    enum ShowMode
    {
		eSM_ReceiverLightSpacePosition,
		eSM_RefractorLightSpaceFrontNorm,
		eSM_RefractorLightSpaceBackNorm,
		eSM_RefractorShadow,
		eSM_Final
    };

    Visualizer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~Visualizer();

    void Initialize(GPUDevice* device,
		ReceiverResourceRenderer* receiverResourceRenderer,
		RefractorResourceRenderer* refractorResourceRenderer,
		RefractorResourceRendererBack* refractorResourceRendererBack,
        Camera* mainCamera);

    void Render(int technique, int pass);

    // Implement base class interface.
    void OnRender(int technique, int pass, Camera* camera);

    void SetShowMode(ShowMode mode);

private:
    ShowMode mShowMode;

    VisualizerScreenQuadPtr mScreenQuad;

    Texture2DPtr mReceiverPositionTexture;
    Texture2DPtr mRefractorFrontNormalTexture;
    Texture2DPtr mRefractorBackNormalTexture;

	/*
    Texture2DPtr mGBufferAlbedoTexture;
    Texture2DArrayPtr mRSMPositionTextureArray;
    Texture2DArrayPtr mRSMNormalTextureArray;
    Texture2DArrayPtr mRSMFluxTextureArray;
    Texture2DPtr mDirectLightingTexture;
    Texture2DPtr mIndirectLightingTexture;
	*/
};

typedef RefPointer<Visualizer> VisualizerPtr;

}

#endif