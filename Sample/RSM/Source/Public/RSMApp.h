#ifndef RTGI_RSMApp_H
#define RTGI_RSMApp_H

#include "RSMTriMesh.h"
#include "RSMTempScreenQuad.h"
#include "RSMDeferredLightingQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 06/15/2014
//----------------------------------------------------------------------------
class RSMApp : public Application
{
public:
	RSMApp(int width = 1024, int height = 768);
	~RSMApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;

private:
    void DrawSceneToRSMBuffer();
	void DrawScene();

	enum ShowMode
	{
		SM_RSMPosition,
		SM_RSMNormal,
		SM_RSMFlux,
		SM_RSMDepth,
		SM_IndirectLighting,
        SM_Position,
        SM_Normal,
        SM_Color,
        SM_Depth,
        SM_DeferredLighting
	};

    Camera* mLightProjector;
	LightPtr mLight;
	RSMTriMeshPtr mGround;
	RSMTriMeshPtr mBackWall;
	RSMTriMeshPtr mLeftWall;
	RSMTriMeshPtr mModel;

	// RSM-buffer.
	FrameBufferPtr mRSMBuffer;
	Texture2DPtr mRSMPositionTexture; // Light view space
	Texture2DPtr mRSMNormalTexture;   // Light view space
	Texture2DPtr mRSMFluxTexture;
	Texture2DPtr mRSMDepthTexture;

	// G-buffer.
	FrameBufferPtr mGBuffer;
    Texture2DPtr mPositionTexture; // World space
    Texture2DPtr mNormalTexture;   // World space
    Texture2DPtr mColorTexture;
    Texture2DPtr mDepthTexture;
	Texture2DPtr mIndirectLightingTexture;

	ShowMode mShowMode;

	RSMTempScreenQuadPtr mRSMTempResultQuad;
    RSMDeferredLightingQuadPtr mRSMDeferredLightingQuad;

	enum { RSM_SAMPLE_COUNT = 400 };
	float randmoNumbers[RSM_SAMPLE_COUNT*3];
	RSMTriMeshPtr mSphere;

	Texture2DPtr mSamplingPatternTexture;
};

}

#endif