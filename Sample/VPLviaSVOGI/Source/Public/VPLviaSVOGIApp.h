#ifndef RTGI_VPLviaSVOGIApp_H
#define RTGI_VPLviaSVOGIApp_H

#include "SceneMesh.h"
#include "GPUTimer.h"
#include "ShadowMapRenderer_Voxel.h"
#include "RSMRenderer.h"
#include "VPLGenerator.h"
#include "DirectLightingRenderer.h"
#include "IndirectLightingRenderer.h"
#include "GridVoxelizer.h"
#include "SVOVoxelizer.h"
#include "Visualizer.h"
#include "FormEventListener.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class VPLviaSVOGI : public Application, GUIFramework::FormEventListener
{
public:
	VPLviaSVOGI(int width = 1024, int height = 768);
	~VPLviaSVOGI();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;
    void OnRadioButtonClick(System::Object^  sender, System::EventArgs^  e) override;
    void OnCheckBoxClick(System::Object^  sender, System::EventArgs^  e) override;
    void OnButtonClick(System::Object^  sender, System::EventArgs^  e) override;

private:
    friend class SceneMesh;
    friend class SVOCubeMesh;

    bool mIsRotatingModel;
    bool mIsWireframe;

    Camera* mLightProjector;

    // GI temporal coherence.
    bool mUseTC;

    // Scene voxelization.
    enum { VOXEL_DIMENSION = 128 };
    enum { VOXEL_LOCAL_GROUP_DIM = 8 };
    Voxelizer::VoxelizerType mVoxelizerType;
    VoxelizerPtr mVoxelizer;

    // G-buffer.
    GBufferRendererPtr mGBufferRenderer;

    // Scene shadow map.
    ShadowMapRendererPtr mShadowMapRenderer;

    // Direct lighting.
    DirectLightingRendererPtr mDirectLightingRenderer;

    // Indirect lighting.
    IndirectLightingRendererPtr mIndirectLightingRenderer;

    // Scene light RSM.
    enum { RSM_FACE_COUNT = 5 };
    RSMRendererPtr mRSMRenderer;

    // Sample RSM.
    VPLGeneratorPtr mVPLGenerator;

    // VPL stuff.
    enum { INTERLEAVED_PATTERN_SIZE = 2 };
    enum { VPL_SAMPLE_COUNT = 256 };

    // Visualizer.
    VisualizerPtr mVisualizer;

    friend class SceneMesh;
    enum SceneModelPass
    {
        SMP_Voxelization = 0,
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
    SceneMeshPtr mModel2;
    RenderSetPtr mSceneObjects;
    RenderSetPtr mVoxelizedObjects;

    AABB mSceneBB;

    GPUTimerPtr mTimer;
};

}

#endif