#ifndef RTGI_BidirectionalVoxelGIApp_H
#define RTGI_BidirectionalVoxelGIApp_H

#include "GraphicsFrameworkHeader.h"
#include "SceneMesh.h"
#include "TempScreenQuad.h"
#include "DirectLightingScreenQuad.h"
#include "IndirectLightingScreenQuad.h"
#include "SampleRSM.h"
#include "GPUTimer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class BidirectionalVoxelGIApp : public Application
{
public:
	BidirectionalVoxelGIApp(int width, int height);
	~BidirectionalVoxelGIApp();

	void Initialize();
	void Run();
	void Terminate();

	// Event handlers.
	void OnKeyboard(unsigned char key, int x, int y);
	void OnKeyboardUp(unsigned char key, int x, int y);
	void OnMouse(int button, int state, int x, int y);
	void OnMouseMove(int x, int y);
	void OnReshape(int x, int y);

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

	void ShadowPass();
    void GBufferPass();
    void RSMPass();
    void VPLShadowPass();

	int mWidth, mHeight;
	std::string mWindowTitle;

	Camera* mCamera;
    Camera* mLightProjector;

    // G-buffer.
    FrameBufferPtr mGBufferFB;
    Texture2DPtr mGBufferPositionTexture;
    Texture2DPtr mGBufferNormalTexture;
    Texture2DPtr mGBufferAlbedoTexture;
    Texture2DPtr mGBufferDepthTexture;

    // Scene shadow map.
    FrameBufferPtr mShadowMapFB;
    Texture2DPtr mShadowMapTexture;
    Texture2DPtr mShadowMapDepthTexture;

    // Direct illumination buffer.
    FrameBufferPtr mDirectLightingFB;
    Texture2DPtr mDirectLightingTexture;
    Texture2DPtr mDirectLightingDepthTexture;

    // Indirect illumination buffer.
    FrameBufferPtr mIndirectLightingFB;
    Texture2DPtr mIndirectLightingTexture;
    Texture2DPtr mIndirectLightingDepthTexture;

    // Scene light RSM.
    enum { RSM_FACE_COUNT = 5 };
    FrameBufferPtr mRSMFB;
    Texture2DArrayPtr mRSMPositionTextureArray;
    Texture2DArrayPtr mRSMNormalTextureArray;
    Texture2DArrayPtr mRSMFluxTextureArray;
    Texture2DArrayPtr mRSMDepthTextureArray;

    // Sample RSM.
    SampleRSMPtr mSampleRSMTask;

    // VPL shadow maps.
    FrameBufferPtr mVPLShadowMapFB;
    Texture2DArrayPtr mVPLShadowMapTextureArray;
    Texture2DArrayPtr mVPLShadowMapDepthTextureArray;

    // VPL stuff.
    enum { VPL_SAMPLE_COUNT = 128 };
    Texture1DPtr mVPLSamplePattern;
    Texture1DPtr mVPLSampleTest;
    StructuredBufferPtr mVPLBuffer;

    TempScreenQuadPtr mTempScreenQuad;
    DirectLightingScreenQuadPtr mDirectLightingScreenQuad;
    IndirectLightingScreenQuadPtr mIndirectLightingScreenQuad;

    // Scene models.
	SceneMeshPtr mGround;
	SceneMeshPtr mCeiling;
    SceneMeshPtr mBackWall;
    SceneMeshPtr mLeftWall;
    SceneMeshPtr mRightWall;
    SceneMeshPtr mModel;

    GPUTimerPtr mTimer;
};

}

#endif