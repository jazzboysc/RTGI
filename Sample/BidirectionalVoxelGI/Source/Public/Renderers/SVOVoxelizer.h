#ifndef RTGI_SVOVoxelizer_H
#define RTGI_SVOVoxelizer_H

#include "GraphicsFrameworkHeader.h"
#include "Voxelizer.h"

namespace RTGI
{

#define RTGI_SVOVoxelizer_SVONodeBuffer_Name "SVONodeBuffer"

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/24/2015
//----------------------------------------------------------------------------
class SVOVoxelizer : public Voxelizer
{
public:
    SVOVoxelizer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~SVOVoxelizer();

    void Initialize(GPUDevice* device, int voxelGridDim, 
        int voxelGridLocalGroupDim, AABB* sceneBB);
    void Render(int technique, int pass);

    // Implement base class interface.
    void OnRender(int technique, int pass, Camera* camera);

private:
    int mVoxelGridLocalGroupDim;
    int mGlobalDim;

    AABB* mSceneBB;
    float mSceneBBMaxLength;
};

typedef RefPointer<SVOVoxelizer> SVOVoxelizerPtr;

}

#endif