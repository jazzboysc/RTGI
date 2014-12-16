#ifndef RTGI_BidirectionalVoxelGIApp_H
#define RTGI_BidirectionalVoxelGIApp_H

#include "SceneMesh.h"
#include "TempScreenQuad.h"
#include "GPUTimer.h"
#include "ShadowMapRenderer.h"
#include "RSMRenderer.h"
#include "VPLGenerator.h"
#include "DirectLightingRenderer.h"
#include "IndirectLightingRenderer.h"
#include "Voxelizer.h"
#include "FormEventListener.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class BidirectionalVoxelGIApp : public Application, GUIFramework::FormEventListener
{
public:
	BidirectionalVoxelGIApp(int width = 1024, int height = 768);
	~BidirectionalVoxelGIApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;
    void OnRadioButtonClick(System::Object^  sender, System::EventArgs^  e);

private:
    friend class SceneMesh;

    enum ShowMode
    {
        SM_Shadow,
        SM_VPLShadow,
        SM_GBufferPosition,
        SM_GBufferNormal,
        SM_GBufferAlbedo,
        SM_RSMPosition,
        SM_RSMNormal,
        SM_RSMFlux,
        SM_DirectLighting,
        SM_IndirectLighting,
        SM_Final
    };

    bool mIsRotatingModel;
    bool mIsWireframe;
    ShowMode mShowMode;

    Camera* mLightProjector;

    // Scene voxelization.
    enum { VOXEL_DIMENSION = 64 };
    enum { VOXEL_LOCAL_GROUP_DIM = 8 };
    VoxelizerPtr mVoxelizer;

    // G-buffer.
    GBufferRendererPtr mGBufferRenderer;
    Texture2DPtr mGBufferPositionTexture;
    Texture2DPtr mGBufferNormalTexture;
    Texture2DPtr mGBufferAlbedoTexture;

    // Scene shadow map.
    ShadowMapRendererPtr mShadowMapRenderer;
    Texture2DPtr mShadowMapTexture;

    // Direct lighting.
    DirectLightingRendererPtr mDirectLightingRenderer;
    Texture2DPtr mDirectLightingTexture;

    // Indirect lighting.
    IndirectLightingRendererPtr mIndirectLightingRenderer;
    Texture2DPtr mIndirectLightingTexture;

    // Scene light RSM.
    enum { RSM_FACE_COUNT = 5 };
    RSMRendererPtr mRSMRenderer;
    Texture2DArrayPtr mRSMPositionTextureArray;
    Texture2DArrayPtr mRSMNormalTextureArray;
    Texture2DArrayPtr mRSMFluxTextureArray;

    // Sample RSM.
    VPLGeneratorPtr mVPLGenerator;

    // VPL stuff.
    enum { VPL_SAMPLE_COUNT = 128 };

    TempScreenQuadPtr mTempScreenQuad;

    friend class SceneMesh;
    enum SceneModelPass
    {
        SMP_Voxelization = 0,
        SMP_ShowVoxelization,
        SMP_ShadowMap,
        SMP_GBuffer,
        SMP_RSM
    };

    // Scene models.
	SceneMeshPtr mGround;
	SceneMeshPtr mCeiling;
    SceneMeshPtr mBackWall;
    SceneMeshPtr mLeftWall;
    SceneMeshPtr mRightWall;
    SceneMeshPtr mModel;
    RenderSetPtr mSceneObjects;

    AABB mSceneBB;

    GPUTimerPtr mTimer;
};

}

#endif