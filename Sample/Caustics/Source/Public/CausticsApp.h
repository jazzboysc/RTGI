#pragma once
#include "CausticsTriMesh.h"
#include "VertexGrid.h"
#include "CausticsCube.h"
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
	void DrawReceiverLightPoV();
	void DrawRefracterLightPoV();
	void DrawReceiverCameraPoV();
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

	// Receiver G-buffer
	FrameBufferPtr mRecvGBuffer;
	Texture2DPtr mRecvPositionTexture;
	Texture2DPtr mRecvNormalTexture;
	Texture2DPtr mRecvColorTexture;
	Texture2DPtr mRecvDepthTexture;

	// Refracter G-buffer
	FrameBufferPtr mRefracGBuffer;
	Texture2DPtr mRefracPositionTexture;
	Texture2DPtr mRefracNormalTexture;
	Texture2DPtr mRefracColorTexture;
	Texture2DPtr mRefracDepthTexture;

	// Intersection map G-buffer
	FrameBufferPtr mIntersectionGBuffer;
	Texture2DPtr mIntersectionPositionTexture;
	Texture2DPtr mIntersectionDepthTexture;

	// caustics map G-buffer
	FrameBufferPtr mCausticsMapGBuffer;
	Texture2DPtr mCausticsMapTexture;
	Texture2DPtr mCausticsMapDepthTexture;


	TextureCubePtr mCubeMap;

	CausticsTriMeshPtr mGround;
	CausticsCubePtr mPool;
	CausticsTriMeshPtr mMesh;
	CausticsScreenQuadPtr mCausticsScreenQuad;
	VertexGridPtr mVertexGrid;



	StructuredBufferPtr mCausticsDebugBuffer;
	struct CausticsDebugBuffer
	{
		// Debug.
		unsigned int value1;
		unsigned int value2;
		unsigned int value3;
		unsigned int value4;
	};

	ShowMode mShowMode;

};

}

