#ifndef RTGI_VPLApp_H
#define RTGI_VPLApp_H

#include "GraphicsFrameworkHeader.h"
#include "VPLTriMesh.h"
#include "VPLTempScreenQuad.h"
#include "VPLDirectLightingScreenQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/21/2014
//----------------------------------------------------------------------------
class VPLApp : public Application
{
public:
	VPLApp(int width, int height);
	~VPLApp();

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
    enum ShowMode
    {
        SM_Shadow,
        SM_GBufferPosition,
        SM_GBufferNormal,
        SM_GBufferAlbedo,
        SM_DirectLighting
    };

    bool mIsWireframe;
    ShowMode mShowMode;

	void ShadowPass();
    void GBufferPass();
    void RSMPass();

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

    // Scene light RSM.
    FrameBufferPtr mRSMFB;
    Texture2DArrayPtr mRSMPositionTextureArray;
    Texture2DArrayPtr mRSMNormalTextureArray;
    Texture2DArrayPtr mRSMDepthTextureArray;

    VPLTempScreenQuadPtr mTempScreenQuad;
    VPLDirectLightingScreenQuadPtr mDirectLightingScreenQuad;

	VPLTriMeshPtr mGround;
	VPLTriMeshPtr mCeiling;
	VPLTriMeshPtr mBackWall;
	VPLTriMeshPtr mLeftWall;
	VPLTriMeshPtr mRightWall;
	VPLTriMeshPtr mModel;
};

}

#endif