#ifndef RTGI_SSDOApp_H
#define RTGI_SSDOApp_H

#include "GraphicsFrameworkHeader.h"
#include "SSDOScreenQuad.h"
#include "SSDOFilterScreenQuad.h"
#include "SSDOTempScreenQuad.h"
#include "DirectLightingQuad.h"
#include "SSDOTriMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/22/2014
//----------------------------------------------------------------------------
class SSDOApp : public Application
{
public:
	SSDOApp(int width, int height);
	~SSDOApp();

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
	void DrawScene();

	enum ShowMode
	{
		SM_Position,
		SM_Normal,
		SM_Color,
		SM_DirectLighting,
		SM_Random,
		SM_SSDO,
		SM_FilteredSSDO
	};

	int mWidth, mHeight;

	std::string mWindowTitle;

	Camera* mCamera;
	LightPtr mLight;
	SSDOTriMeshPtr mModel1;
	SSDOTriMeshPtr mModel2;
	SSDOTriMeshPtr mGround;
	enum { CubeCount = 256 };
	SSDOTriMeshPtr mCubes[CubeCount];

	// G-buffer.
	FrameBufferPtr mGBuffer;
	Texture2DPtr mPositionTexture;
	Texture2DPtr mNormalTexture;
	Texture2DPtr mColorTexture;
	Texture2DPtr mDepthTexture;

	// Direct lighting render target.
	FrameBufferPtr mDirectLightingBuffer;
	Texture2DPtr mDirectLightingTexture;

	// SSDO render target.
	FrameBufferPtr mSSDOBuffer;
	Texture2DPtr mSSDOTexture;

	// Environment map.
	Texture2DPtr mEnvTexture;

	// LD random texture.
	Texture2DPtr mRandomTexture;

	ShowMode mShowMode;

	SSDOScreenQuadPtr mSSDOQuad;
	SSDOFilterScreenQuadPtr mSSDOFilterQuad;
	SSDOTempScreenQuadPtr mSSDOTempResultQuad;
	DirectLightingQuadPtr mDirectLightingQuad;
};

}

#endif