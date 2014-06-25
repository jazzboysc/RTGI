#ifndef RTGI_RSMApp_H
#define RTGI_RSMApp_H

#include "GraphicsFrameworkHeader.h"
#include "RSMTriMesh.h"
#include "RSMTempScreenQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 06/15/2014
//----------------------------------------------------------------------------
class RSMApp : public Application
{
public:
	RSMApp(int width, int height);
	~RSMApp();

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
    void DrawSceneToRSMBuffer();
	void DrawScene();

	enum ShowMode
	{
		SM_Position,
		SM_Normal,
		SM_Flux,
		SM_Depth,
		SM_DirectLighting
	};

	int mWidth, mHeight;

	std::string mWindowTitle;

    Camera* mLightProjector;
	Camera* mCamera;
	LightPtr mLight;
	RSMTriMeshPtr mGround;
	RSMTriMeshPtr mBackWall;
	RSMTriMeshPtr mLeftWall;
	RSMTriMeshPtr mModel;

	// RSM-buffer.
	FrameBufferPtr mRSMBuffer;
	Texture2DPtr mPositionTexture;
	Texture2DPtr mNormalTexture;
	Texture2DPtr mFluxTexture;
	Texture2DPtr mDepthTexture;

	// Direct lighting render target.
	FrameBufferPtr mDirectLightingBuffer;
	Texture2DPtr mDirectLightingTexture;

	ShowMode mShowMode;

	RSMTempScreenQuadPtr mRSMTempResultQuad;

	enum { RSM_SAMPLE_COUNT = 400 };
	float randmoNumbers[RSM_SAMPLE_COUNT*3];
	RSMTriMeshPtr mSphere;

	Texture2DPtr mSamplingPatternTexture;
};

}

#endif