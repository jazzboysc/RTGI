#pragma once
#include "AdaptiveCausticsTriMesh.h"
#include "AdaptiveCausticsCube.h"
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
	AdaptiveCausticsApp(int width = 1024, int height = 768);
	~AdaptiveCausticsApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;

private:
	void InitializeTextures();
	void InitializeScene();
	void InitializeFBO();

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
		RenderSet receiver;
		RenderSet refractor;

		CausticsTriMeshPtr mesh;
		CausticsCubePtr pool;
		CausticsTriMeshPtr ground;

	} mScene;
	
	struct FBO
	{
		FrameBufferPtr backgroundObjectsPositionLightSpace;
		FrameBufferPtr refractorNormalFrontBackLightSpace;
		FrameBufferPtr a;
		FrameBufferPtr b;
		FrameBufferPtr c;

	} mFBO;

	LightPtr mLight;

	TextureCubePtr mCubeMap;

	StructuredBufferPtr mCausticsDebugBuffer;
	GPUTimerPtr mTimer;
};

}

