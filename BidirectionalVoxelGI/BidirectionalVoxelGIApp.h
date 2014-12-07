#ifndef RTGI_BidirectionalVoxelGIApp_H
#define RTGI_BidirectionalVoxelGIApp_H

#include "GraphicsFrameworkHeader.h"
#include "SceneMesh.h"
#include "TempScreenQuad.h"
#include "GPUTimer.h"
#include "ShadowMapRenderer.h"
#include "RSMRenderer.h"
#include "VPLGenerator.h"
#include "DirectLightingRenderer.h"
#include "IndirectLightingRenderer.h"

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

	int mWidth, mHeight;
	std::string mWindowTitle;

	Camera* mCamera;
    Camera* mLightProjector;

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
    StructuredBufferPtr mVPLBuffer;

    TempScreenQuadPtr mTempScreenQuad;

    // Scene models.
	SceneMeshPtr mGround;
	SceneMeshPtr mCeiling;
    SceneMeshPtr mBackWall;
    SceneMeshPtr mLeftWall;
    SceneMeshPtr mRightWall;
    SceneMeshPtr mModel;
    RenderSetPtr mSceneObjects;

    GPUTimerPtr mTimer;
};

}

#endif