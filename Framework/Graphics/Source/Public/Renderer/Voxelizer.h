//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Voxelizer_H
#define RTGI_Voxelizer_H

#include "SubRenderer.h"

namespace RTGI
{

enum VoxelizerType
{
    VT_Grid,
    VT_SVO
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/23/2015
//----------------------------------------------------------------------------
class Voxelizer : public SubRenderer
{
public:
    Voxelizer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~Voxelizer();

    int RasterizerDimBias;
    int VoxelGridDim;

    inline VoxelizerType GetVoxelizerType() const;

protected:
    VoxelizerType mVoxelizerType;
};

typedef RefPointer<Voxelizer> VoxelizerPtr;

#include "Voxelizer.inl"

}

#endif