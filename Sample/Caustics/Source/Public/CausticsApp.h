#pragma once
#include "CausticsTriMesh.h"
#include "CausticsScreenQuad.h"
namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/22/2014
//----------------------------------------------------------------------------
class CausticsApp : public Application
{
public:
	CausticsApp(int width = 1024, int height = 768);
	~CausticsApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;

private:
	void DrawScene();

	enum ShowMode
	{
		SM_Position,
		SM_Normal,
		SM_Color,
		SM_DirectLighting,
		SM_Random
	};

	LightPtr mLight;

	// G-buffer.
	FrameBufferPtr mGBuffer;
	Texture2DPtr mPositionTexture;
	Texture2DPtr mNormalTexture;
	Texture2DPtr mColorTexture;
	Texture2DPtr mDepthTexture;

	CausticsTriMeshPtr mGround;
	CausticsTriMeshPtr mMesh;
	CausticsScreenQuadPtr mCausticsScreenQuad;

	ShowMode mShowMode;

};

}

