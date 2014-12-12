#ifndef RTGI_SIIApp_H
#define RTGI_SIIApp_H

#include "GraphicsFrameworkHeader.h"
#include "SIITriMesh.h"
#include "SIITempScreenQuad.h"
#include "SIIVPLQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/12/2014
//----------------------------------------------------------------------------
class SIIApp : public Application
{
public:
	SIIApp(int width, int height);
	~SIIApp();

	void Initialize(GPUDevice* device);
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
        SM_RSMPosition,
        SM_RSMNormal,
        SM_RSMFlux,
        SM_Scene
    };

    bool mIsWireframe;
    ShowMode mShowMode;

	void DrawShadow();
    void DrawRSMs();
    void SampleRSMs();
    void DrawScene();

	int mWidth, mHeight;
	std::string mWindowTitle;

	Camera* mCamera;
    Camera* mLightProjector;

    FrameBufferPtr mShadowMapFB;
    Texture2DPtr mShadowMapTexture;
    Texture2DPtr mShadowMapDepthTexture;

    // RSM-buffer.
    FrameBufferPtr mRSMBufferPX;
    Texture2DPtr mRSMPositionTexturePX;
    Texture2DPtr mRSMNormalTexturePX;
    Texture2DPtr mRSMFluxTexturePX;
    Texture2DPtr mRSMDepthTexturePX;

    Texture1DPtr mRSMSampleTexture;

    // VPL quad.
    SIIVPLQuadPtr mVPLQuad;

    SIITempScreenQuadPtr mShadowMapScreenQuad;

	SIITriMeshPtr mGround;
	SIITriMeshPtr mCeiling;
	SIITriMeshPtr mBackWall;
	SIITriMeshPtr mLeftWall;
	SIITriMeshPtr mRightWall;
	SIITriMeshPtr mModel;
};

}

#endif