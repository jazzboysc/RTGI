#ifndef RTGI_Voxelizer_H
#define RTGI_Voxelizer_H

#include "GraphicsFrameworkHeader.h"

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
class Voxelizer : public SubRenderer
{
public:
    Voxelizer(RenderSet* renderSet = 0);
    virtual ~Voxelizer();

    void Initialize(GPUDevice* device, int voxelGridDim, 
        int voxelGridLocalGroupDim, AABB* sceneBB);
    void Render(int technique, int pass);

    // Implement base class interface.
    void OnRender(int technique, int pass, Camera* camera);

    int GetVoxelGridDim() const;
    int GetVoxelGridLocalGroupDim() const;

    int RasterizerDimBias;

private:
    int mVoxelGridDim;
    int mVoxelGridLocalGroupDim;
    int mGlobalDim;

    AABB* mSceneBB;
    float mSceneBBMaxLength;

    ResetVoxelBufferPtr mResetVoxelBufferTask;
};

typedef RefPointer<Voxelizer> VoxelizerPtr;

}

#endif