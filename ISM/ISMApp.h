#ifndef RTGI_ISMApp_H
#define RTGI_ISMApp_H

#include "GraphicsFrameworkHeader.h"
#include "ISMTriMesh.h"
#include "ISMTempScreenQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/05/2014
//----------------------------------------------------------------------------
class ISMApp : public Application
{
public:
	ISMApp(int width, int height);
	~ISMApp();

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
        SM_Scene
    };

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

    ISMTempScreenQuadPtr mShadowMapScreenQuad;

	ISMTriMeshPtr mGround;
	ISMTriMeshPtr mCeiling;
	ISMTriMeshPtr mBackWall;
	ISMTriMeshPtr mLeftWall;
	ISMTriMeshPtr mRightWall;
	ISMTriMeshPtr mModel;
};

}

#endif