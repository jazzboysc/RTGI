#ifndef RTGI_SVOApp_H
#define RTGI_SVOApp_H

#include "SVOTriMesh.h"
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
class SVOApp : public Application, GUIFramework::FormEventListener
{
public:
	SVOApp(int width = 768, int height = 768);
	~SVOApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;
    void OnButtonClick(System::Object^  sender, System::EventArgs^  e);

public:
    enum { VOXEL_DIMENSION = 128 };
    enum { LOCAL_GROUP_DIM = 8 };
    static float RaySegment[6];

private:
    void VoxelizeScene();
    void ShowVoxelization();

    friend class SVOTriMesh;
    enum ShowMode
    {
        SM_VoxelGrid,
        SM_Scene,
        SM_WorldPosition
    };

    ShowMode mShowMode;
    bool mIsRotatingModel;

    AtomicCounterBufferPtr mAtomicCounterBuffer;
    ResetVoxelBufferPtr mResetVoxelBufferTask;
    GatherVoxelBufferPtr mGatherVoxelBufferTask;
    VoxelGridIntersectionPtr mVoxelGridIntersectionTask;
    StructuredBufferPtr mVoxelBuffer;
    StructuredBufferPtr mIndirectCommandBuffer;
    GLuint mZeroBuffer[VOXEL_DIMENSION*VOXEL_DIMENSION*VOXEL_DIMENSION*4];
    AABB mSceneBB;

	SVOTriMeshPtr mGround;
	SVOTriMeshPtr mCeiling;
	SVOTriMeshPtr mBackWall;
	SVOTriMeshPtr mLeftWall;
	SVOTriMeshPtr mRightWall;
	SVOTriMeshPtr mModel;
    VoxelCubeTriMeshPtr mVoxelCubeModel;
    VoxelRaySegmentPtr mVoxelRaySegment;

    GPUTimerPtr mTimer;
};

}

#endif