#ifndef RTGI_SSSviaPSMApp_H
#define RTGI_SSSviaPSMApp_H

#include "GraphicsFrameworkHeader.h"
#include "SSSviaPSMTriMesh.h"
#include "SSSviaPSMTempScreenQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/09/2014
//----------------------------------------------------------------------------
class SSSviaPSMApp : public Application
{
public:
	SSSviaPSMApp(int width, int height);
	~SSSviaPSMApp();

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
        SM_Scene
    };

    bool mIsRotatingModel;
    bool mIsWireframe;
    ShowMode mShowMode;

	void DrawShadow();
    void DrawScene();

	int mWidth, mHeight;
	std::string mWindowTitle;

	Camera* mCamera;
    Camera* mLightProjector;

    Texture2DPtr mShadowMapTexture;
    Texture2DPtr mShadowMapDepthTexture;
    FrameBufferPtr mShadowMapFB;

    SSSviaPSMTempScreenQuadPtr mShadowMapScreenQuad;

	SSSviaPSMTriMeshPtr mGround;
	SSSviaPSMTriMeshPtr mCeiling;
	SSSviaPSMTriMeshPtr mBackWall;
	SSSviaPSMTriMeshPtr mLeftWall;
	SSSviaPSMTriMeshPtr mRightWall;
	SSSviaPSMTriMeshPtr mModel;
};

}

#endif