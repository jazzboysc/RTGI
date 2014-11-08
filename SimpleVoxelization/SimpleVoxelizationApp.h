#ifndef RTGI_SimpleVoxelizationApp_H
#define RTGI_SimpleVoxelizationApp_H

#include "GraphicsFrameworkHeader.h"
#include "SimpleVoxelizationTriMesh.h"
#include "VoxelCubeTriMesh.h"
#include "GPUTimer.h"
#include "ResetVoxelBuffer.h"
#include "GatherVoxelBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/08/2014
//----------------------------------------------------------------------------
class SimpleVoxelizationApp : public Application
{
public:
	SimpleVoxelizationApp(int width, int height);
	~SimpleVoxelizationApp();

	void Initialize();
	void Run();
	void Terminate();

	// Event handlers.
	void OnKeyboard(unsigned char key, int x, int y);
	void OnKeyboardUp(unsigned char key, int x, int y);
	void OnMouse(int button, int state, int x, int y);
	void OnMouseMove(int x, int y);
	void OnReshape(int x, int y);

public:
    enum { VOXEL_DIMENSION = 64 };
    enum { LOCAL_GROUP_DIM = 8 };

private:
    void VoxelizeScene();
    void ShowVoxelization();

    enum ShowMode
    {
        SM_VoxelGrid,
        SM_Scene,
    };

    ShowMode mShowMode;
	int mWidth, mHeight;
	std::string mWindowTitle;

    bool mIsRotatingModel;

    Camera* mCamera;
	Camera* mVoxelizationProjector;

    AtomicCounterBufferPtr mGatheredVoxelAllocCounter;
    ResetVoxelBufferPtr mResetVoxelBufferTask;
    GatherVoxelBufferPtr mGatherVoxelBufferTask;
    StructuredBufferPtr mVoxelBuffer;
    StructuredBufferPtr mIndirectCommandBuffer;
    GLuint mZeroBuffer[VOXEL_DIMENSION*VOXEL_DIMENSION*VOXEL_DIMENSION*4];
    AABB mSceneBB;

	SimpleVoxelizationTriMeshPtr mGround;
	SimpleVoxelizationTriMeshPtr mCeiling;
	SimpleVoxelizationTriMeshPtr mBackWall;
	SimpleVoxelizationTriMeshPtr mLeftWall;
	SimpleVoxelizationTriMeshPtr mRightWall;
	SimpleVoxelizationTriMeshPtr mModel;
    VoxelCubeTriMeshPtr mVoxelCubeModel;

    GPUTimerPtr mTimer;
};

}

#endif