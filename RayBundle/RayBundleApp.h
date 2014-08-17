#ifndef RTGI_RayBundleApp_H
#define RTGI_RayBundleApp_H

#include "GraphicsFrameworkHeader.h"
#include "RayBundleTriMesh.h"
#include "UpdateAccumulationScreenQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/31/2014
//----------------------------------------------------------------------------
class RayBundleApp : public Application
{
public:
	RayBundleApp(int width, int height);
	~RayBundleApp();

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
	void DrawRayBundle();
	void DrawScene();

	int mWidth, mHeight;
	std::string mWindowTitle;

    Camera* mRayBundleProjector;
	Camera* mCamera;

	RayBundleTriMeshPtr mGround;
	RayBundleTriMeshPtr mCeiling;
	RayBundleTriMeshPtr mLight;
	RayBundleTriMeshPtr mBackWall;
	RayBundleTriMeshPtr mLeftWall;
	RayBundleTriMeshPtr mRightWall;
	RayBundleTriMeshPtr mModel;

	AtomicCounterBufferPtr mRayAllocCounter;
	Texture2DPtr mRayHeadPointerTexture;
	PixelBufferPtr mRayHeadPointerTextureInitData;
	Texture1DPtr mPerVoxelMutexTexture;
	PixelBufferPtr mPerVoxelMutexTextureInitData;
	StructuredBufferPtr mRayBundleNodeBuffer;

	int mVoxelGridDim;
	int mVoxelCount;
	StructuredBufferPtr mAccumulationBuffer;

	int mRayBundleRTWidth, mRayBundleRTHeight;
	FrameBufferPtr mRayBundleFB;
	Texture2DPtr mRayBundleRT;
	Texture2DPtr mRayBundleDepth;

	UpdateAccumulationScreenQuadPtr mUpdateAccuScreenQuad;
	AABB mSceneBB;
	vec3 mVoxelGridCenter;

	float sphereRadius;
	vec3 sphereCenter;
	enum { RAYBUNDLE_SAMPLE_COUNT = 32 };
	vec3 mRandmoDirections[RAYBUNDLE_SAMPLE_COUNT];
};

}

#endif