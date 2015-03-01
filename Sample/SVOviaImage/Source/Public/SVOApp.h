#ifndef RTGI_SVOApp_H
#define RTGI_SVOApp_H

#include "SVOTriMesh.h"
#include "SVOCubeMesh.h"
#include "SVORaySegment.h"
#include "SVORayIntersection.h"
#include "GPUTimer.h"
#include "GatherVoxelFragmentListInfo.h"
#include "BuildSVO.h"
#include "BufferTexture.h"
#include "InformationPanel.h"
#include "FormEventListener.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/26/2015
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
    void OnCheckBoxClick(System::Object^  sender, System::EventArgs^  e) override;

public:
    enum { VOXEL_DIMENSION = 256 };
    enum { LOCAL_GROUP_DIM = 8 };
    static float RaySegment[6];

private:
    void VoxelizeScene();
    void ShowVoxelization();

    friend class SVOTriMesh;
    enum ShowMode
    {
        SM_Scene,
        SM_WorldPosition
    };

    ShowMode mShowMode;
    bool mIsRotatingModel;
    bool mVoxelizeCornell;
    bool mShowCornell;
    bool mShowSVOBoxBV;

    AtomicCounterBufferPtr mAtomicCounterBuffer;
    GatherVoxelFragmentListInfoPtr mGatherVoxelFragmentListInfoTask;
    BuildSVOPtr mBuildSVOTask;
    StructuredBufferPtr mVoxelFragmentListBuffer;
    StructuredBufferPtr mSVOCommandBuffer;
    TextureBufferPtr mSVONodeBuffer;
    BufferTexturePtr mSVONodeBufferTexture;
    UniformBufferPtr mSVOUniformBuffer;
    SVORayIntersectionPtr mSVORayIntersectionTask;

    AABB mSceneBB;
    unsigned int mSVONodeCount;
    unsigned int mSVOMaxLevel;

	SVOTriMeshPtr mGround;
	SVOTriMeshPtr mCeiling;
	SVOTriMeshPtr mBackWall;
	SVOTriMeshPtr mLeftWall;
	SVOTriMeshPtr mRightWall;
	SVOTriMeshPtr mModel;
    SVOCubeMeshPtr mSVONodeCubeModel;
    SVORaySegmentPtr mSVORaySegment;

    GPUTimerPtr mTimer;
};

}

#endif