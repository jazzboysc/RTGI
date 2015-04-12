#pragma once
#include "AdaptiveCausticsTriMesh.h"
#include "AdaptiveCausticsCube.h"

#include "ReceiverResourceRenderer.h"
#include "RefractorResourceRenderer.h"
#include "ShadowMapRenderer.h"
#include "CausticMapRenderer.h"
#include "Visualizer.h"

#include "GPUTimer.h"
#include "FormEventListener.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/22/2014
//----------------------------------------------------------------------------
class AdaptiveCausticsApp : public Application, GUIFramework::FormEventListener
{
public:
	AdaptiveCausticsApp(int width = 768, int height = 768);
	~AdaptiveCausticsApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;

	void OnRadioButtonClick(System::Object^  sender, System::EventArgs^  e) override;
	void OnCheckBoxClick(System::Object^  sender, System::EventArgs^  e) override;

private:
	void InitializeTextures();
	void InitializeScene();
	void InitializeFBO();

	bool bIsSpinningMesh = false;

	ReceiverResourceRendererPtr mReceiverResourceRenderer;
	RefractorResourceRendererPtr mRefractorResourceRenderer;
	ShadowMapRendererPtr mShadowMapRenderer;
	CausticsMapRendererPtr mCausticMapRenderer;
	VisualizerPtr mVisualizer;

	struct DebugBuffer
	{
		// Debug.
		unsigned int value1;
		unsigned int value2;
		unsigned int value3;
		unsigned int value4;

	} mDebugBuffer;

	struct Textures
	{
		Texture2DPtr backgroundObjectsPositionLightSpace;

	} mTextures;

	struct Scene
	{
		RenderSetPtr receiver;
		RenderSetPtr refractor;
		RenderSetPtr all;

		CausticsTriMeshPtr mesh;
		AdaptiveCausticsCubePtr pool;
		CausticsTriMeshPtr ground;

	} mScene;
	
	friend class AdaptiveCausticsTriMesh;
	friend class AdaptiveCausticsCube;
	enum SceneModelPass
	{
		SMP_Resource = 0,
		SMP_ShadowMap,
		SMP_AdaptiveCaustics
	};

	Camera* mLightProjector;
	LightPtr mLight;

	TextureCubePtr mCubeMap;

	StructuredBufferPtr mCausticsDebugBuffer;
	GPUTimerPtr mTimer;
};

}

