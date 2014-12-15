#ifndef RTGI_Voxelizer_H
#define RTGI_Voxelizer_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class Voxelizer : public SubRenderer
{
public:
    Voxelizer(RenderSet* renderSet = 0);
    virtual ~Voxelizer();

    void Initialize(int voxelGridDim, int voxelGridLocalGroupDim);

    int GetVoxelGridDim() const;
    int GetVoxelGridLocalGroupDim() const;

private:
    int mVoxelGridDim;
    int mVoxelGridLocalGroupDim;
};

typedef RefPointer<Voxelizer> VoxelizerPtr;

}

#endif