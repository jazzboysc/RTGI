#ifndef RTGI_GridVoxelizer_H
#define RTGI_GridVoxelizer_H

#include "GraphicsFrameworkHeader.h"
#include "Voxelizer.h"

namespace RTGI
{

#define RTGI_Voxelizer_VoxelBuffer_Name "VoxelBuffer"

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/05/2014
//----------------------------------------------------------------------------
class ResetVoxelBuffer : public ComputeTask
{
public:
    ResetVoxelBuffer();
    ~ResetVoxelBuffer();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);
};

typedef RefPointer<ResetVoxelBuffer> ResetVoxelBufferPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class GridVoxelizer : public Voxelizer
{
public:
    GridVoxelizer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~GridVoxelizer();

    void Initialize(GPUDevice* device, int voxelGridDim, 
        int voxelGridLocalGroupDim, AABB* sceneBB);

    // Implement base class interface.
    void OnRender(int technique, int pass, Camera* camera);

private:
    int mVoxelGridLocalGroupDim;
    int mGlobalDim;
    ResetVoxelBufferPtr mResetVoxelBufferTask;
};

typedef RefPointer<GridVoxelizer> GridVoxelizerPtr;

}

#endif