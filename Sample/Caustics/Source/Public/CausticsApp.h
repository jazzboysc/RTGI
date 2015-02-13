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
	void DrawRefracCameraPoV();

	enum ShowMode
	{
		SM_Position,
		SM_Normal,
		SM_Color,
		SM_DirectLighting,
		SM_Random
	};

	LightPtr mLight;

	// G-buffer receiver
	FrameBufferPtr mReceiverGBuffer;
	Texture2DPtr mReceiverPositionTexture;
	Texture2DPtr mReceiverNormalTexture;
	Texture2DPtr mReceiverColorTexture;
	Texture2DPtr mReceiverDepthTexture;

	// G-buffer refractive obj
	FrameBufferPtr mRefracGBuffer;
	Texture2DPtr mRefracPositionTexture;
	Texture2DPtr mRefracNormalTexture;
	Texture2DPtr mRefracColorTexture;
	Texture2DPtr mRefracDepthTexture;

	// Receiver G-buffer light
	FrameBufferPtr mRecvGBufferLight;
	Texture2DPtr mLightReceiverPositionTexture;
	Texture2DPtr mLightReceiverNormalTexture;
	Texture2DPtr mLightReceiverColorTexture;
	Texture2DPtr mLightReceiverDepthTexture;

	// Refracter G-buffer light
	FrameBufferPtr mRefracGBufferLight;
	Texture2DPtr mLightRefracPositionTexture;
	Texture2DPtr mLightRefracNormalTexture;
	Texture2DPtr mLightRefracColorTexture;
	Texture2DPtr mLightRefracDepthTexture;

	// G-buffer for final image
	FrameBufferPtr mGBufferFinal;
	Texture2DPtr mReceiverTexture;
	Texture2DPtr mRefracTexture;
	Texture2DPtr mDepthTexture;

	// Intersection map G-buffer
	FrameBufferPtr mIntersectionGBuffer;
	Texture2DPtr mIntersectionPositionTexture;
	Texture2DPtr mIntersectionDepthTexture;

	// caustics map G-buffer
	FrameBufferPtr mCausticsMapGBuffer;
	Texture2DPtr mCausticsMapTexture;
	Texture2DPtr mCausticsMapDepthTexture;

	// horizontal blurred caustics map frame buffer
	FrameBufferPtr mBlurredCausticsMapGBuffer;
	Texture2DPtr mBlurredCausticsMapTexture;
	Texture2DPtr mBlurredCausticsMapDepthTexture;

	// final blurred caustics map frame buffer
	FrameBufferPtr mBlurredCausticsMapGBuffer2;
	Texture2DPtr mBlurredCausticsMapTexture2;
	Texture2DPtr mBlurredCausticsMapDepthTexture2;

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

