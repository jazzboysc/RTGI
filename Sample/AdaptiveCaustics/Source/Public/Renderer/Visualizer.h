#ifndef RTGI_Visualizer_H
#define RTGI_Visualizer_H

#include "GraphicsFrameworkHeader.h"
#include "ReceiverResourceRenderer.h"
#include "RefractorResourceRenderer.h"
#include "CausticMapRenderer.h"
#include "DirectLightingRenderer.h"
#include "DeferredRefractionRenderer.h"

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
	ShaderUniform mTempArraySamplerLoc;
};

typedef RefPointer<VisualizerScreenQuad> VisualizerScreenQuadPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class Visualizer : public SubRenderer
{
public:
    enum eShowMode
    {
		eSM_ReceiverGBufferPosition,
		eSM_ReceiverGBufferNormal,
		eSM_ReceiverGBufferAlbedo,
		eSM_ReceiverLightSpacePosition,
		eSM_RefractorLightSpaceFrontNorm,
		eSM_RefractorLightSpaceBackNorm,
		eSM_RefractorShadow,
		eSM_CausticMapTraversal,
		eSM_CausticMapSplat,
		eSM_DirectLighting,
		eSM_DeferredRefraction,
		eSM_Final
    };

    Visualizer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~Visualizer();

	void Initialize(GPUDevice* device,
		GBufferRenderer* gBufferRenderer,
		ReceiverResourceRenderer* receiverResourceRenderer,
		RefractorResourceRenderer* refractorResourceRenderer,
		ShadowMapRenderer* shadowMapRenderer,
		CausticMapRenderer* causticMapRenderer,
		DirectLightingRenderer* directLightingRenderer,
		DeferredRefractionRenderer* deferredRefractionRenderer,
        Camera* mainCamera);

    void Render(int technique, int pass);

    // Implement base class interface.
    void OnRender(int technique, int pass, Camera* camera);

    void SetShowMode(eShowMode mode);

private:
    eShowMode mShowMode;

	PipelineStateBlockPtr mPSB;
    VisualizerScreenQuadPtr mScreenQuad;

	Texture2DPtr mScenePositionTexture;
	Texture2DPtr mSceneNormalTexture;
	Texture2DPtr mSceneAlbedoTexture;

    Texture2DPtr mReceiverPositionTexture;
    Texture2DPtr mRefractorFrontAndBackNormalTextures;
	Texture2DPtr mShadowMapTexture;
	Texture2DPtr mComputeTempTexture;
	Texture2DPtr mCausticMapTexture;
	Texture2DPtr mDirectLightingTexture;
	Texture2DPtr mDeferredRefractionTexture;

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