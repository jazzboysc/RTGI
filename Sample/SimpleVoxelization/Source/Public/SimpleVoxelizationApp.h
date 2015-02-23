#ifndef RTGI_SimpleVoxelizationApp_H
#define RTGI_SimpleVoxelizationApp_H

#include "SimpleVoxelizationTriMesh.h"
#include "VoxelCubeTriMesh.h"
#include "VoxelRaySegment.h"
#include "GPUTimer.h"
#include "ResetVoxelBuffer.h"
#include "GatherVoxelBuffer.h"
#include "VoxelGridIntersection.h"
#include "InformationPanel.h"
#include "FormEventListener.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/08/2014
//----------------------------------------------------------------------------
class SimpleVoxelizationApp : public Application, GUIFramework::FormEventListener
{
public:
	SimpleVoxelizationApp(int width = 1024, int height = 768);
	~SimpleVoxelizationApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;
    void OnButtonClick(System::Object^  sender, System::EventArgs^  e);

public:
    enum { VOXEL_DIMENSION = 256 };
    enum { LOCAL_GROUP_DIM = 8 };
    static float RaySegment[6];

private:
    void VoxelizeScene();
    void ShowVoxelization();

    friend class SimpleVoxelizationTriMesh;
    enum ShowMode
    {
        SM_VoxelGrid,
        SM_Scene,
        SM_WorldPosition
    };

    ShowMode mShowMode;
    bool mIsRotatingModel;

	Camera* mVoxelizationProjector;

    AtomicCounterBufferPtr mGatheredVoxelAllocCounter;
    ResetVoxelBufferPtr mResetVoxelBufferTask;
    GatherVoxelBufferPtr mGatherVoxelBufferTask;
    VoxelGridIntersectionPtr mVoxelGridIntersectionTask;
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
    VoxelRaySegmentPtr mVoxelRaySegment;

    GPUTimerPtr mTimer;
};

}

#endif