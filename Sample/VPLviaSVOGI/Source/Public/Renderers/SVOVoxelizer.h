#ifndef RTGI_SVOVoxelizer_H
#define RTGI_SVOVoxelizer_H

#include "GraphicsFrameworkHeader.h"
#include "ComputeTask.h"
#include "Voxelizer.h"

namespace RTGI
{

#define BUILD_SVO_INIT_ROOT_PASS        0
#define BUILD_SVO_FLAG_NODES_PASS       1
#define BUILD_SVO_ALLOC_NODES_PASS      2
#define BUILD_SVO_POST_ALLOC_NODES_PASS 3
#define BUILD_SVO_INIT_NODES_PASS       4
#define BUILD_SVO_SPLAT_LEAF_NODES_PASS 5

#define RTGI_SVOVoxelizer_SVONodeBuffer_Name "SVONodeBuffer"

//----------------------------------------------------------------------------
// These structs are used for debugging SVO node buffer.
//----------------------------------------------------------------------------
struct SVONodeAABB
{
    unsigned int Min;
    unsigned int Max;
};
//----------------------------------------------------------------------------
struct SVONode
{
    unsigned int info; // 31 : leaf bit, 30 : flag bit, 29 - 0 : children id.
    unsigned int userData;
    SVONodeAABB nodeBox;
};
//----------------------------------------------------------------------------
struct SVONodeTile
{
    SVONode children[8];
};
//----------------------------------------------------------------------------
struct SVONodeBufferHead
{
    // Indirect command buffer data for allocation of SVO node tile pass.
    unsigned int  allocThreadCountForCurLevel;
    unsigned int  instanceCount;
    unsigned int  first;
    unsigned int  baseInstance;

    // Indirect command buffer data for visualizing SVO nodes.
    unsigned int  ic2Count;
    unsigned int  ic2PrimCount;
    unsigned int  ic2FirstIndex;
    unsigned int  ic2BaseVertex;
    unsigned int  ic2BaseInstance;

    unsigned int offset0;
    unsigned int offset1;
    unsigned int offset2;

    // Shared data between SVO passes.
    unsigned int rootFlag;
    unsigned int rootChild;
    unsigned int curLevelStartIndex;
    unsigned int curLevelEndIndex;
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Voxel fragment list buffer.
//----------------------------------------------------------------------------
struct VoxelFragment
{
    unsigned int gridPosition;
    unsigned int albedo;
    unsigned int value1;
    unsigned int value2;
};
//----------------------------------------------------------------------------
struct VoxelFragmentBufferHead
{
    // Indirect command buffer data for SVO flag nodes pass.
    unsigned int  count;
    unsigned int  instanceCount;
    unsigned int  first;
    unsigned int  baseInstance;

    // Scene bounding box.
    vec4 SceneBBMin;
    vec4 SceneBBCenter;
    vec4 SceneBBExtension;
    vec4 Inv2SceneBBExtension;
};
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/27/2015
//----------------------------------------------------------------------------
class BuildSVO : public ComputeTask
{
public:
    BuildSVO();
    ~BuildSVO();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

};

typedef RefPointer<BuildSVO> BuildSVOPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/26/2015
//----------------------------------------------------------------------------
class GatherVoxelFragmentListInfo : public ComputeTask
{
public:
    GatherVoxelFragmentListInfo();
    ~GatherVoxelFragmentListInfo();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

};

typedef RefPointer<GatherVoxelFragmentListInfo> GatherVoxelFragmentListInfoPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/24/2015
//----------------------------------------------------------------------------
class SVOVoxelizer : public Voxelizer
{
public:
    SVOVoxelizer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~SVOVoxelizer();

    void Initialize(GPUDevice* device, int voxelGridDim, AABB* sceneBB);

    // Implement base class interface.
    void OnRender(int technique, int pass, Camera* camera);

    StructuredBuffer* GetVoxelFragmentListBuffer() const;
    StructuredBuffer* GetSVOBuffer() const;
    UniformBuffer* GetSVOUniformBuffer() const;

private:
    AtomicCounterBufferPtr mAtomicCounterBuffer;
    GatherVoxelFragmentListInfoPtr mGatherVoxelFragmentListInfoTask;
    BuildSVOPtr mBuildSVOTask;
    StructuredBufferPtr mVoxelFragmentListBuffer;
    StructuredBufferPtr mSVOBuffer;
    UniformBufferPtr mSVOUniformBuffer;

    unsigned int mSVONodeMaxCount;
    unsigned int mSVOMaxLevel;
};

typedef RefPointer<SVOVoxelizer> SVOVoxelizerPtr;

}

#endif