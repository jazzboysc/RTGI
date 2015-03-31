//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Voxelizer_H
#define RTGI_Voxelizer_H

#include "SubRenderer.h"
#include "AABB.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/23/2015
//----------------------------------------------------------------------------
class Voxelizer : public SubRenderer
{
public:
    enum VoxelizerType
    {
        VT_Grid = 0,
        VT_SVO,
        VT_Unknown
    };

    Voxelizer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~Voxelizer();

    void Render(int technique, int pass);

    int RasterizerDimBias;
    int VoxelGridDim;

    inline VoxelizerType GetVoxelizerType() const;
    float GetSceneBBMaxLength() const;

protected:
    void VoxelizeScene(int technique, int pass);

    VoxelizerType mVoxelizerType;
    AABB* mSceneBB;
    float mSceneBBMaxLength;
};

typedef RefPointer<Voxelizer> VoxelizerPtr;

#include "Voxelizer.inl"

}

#endif