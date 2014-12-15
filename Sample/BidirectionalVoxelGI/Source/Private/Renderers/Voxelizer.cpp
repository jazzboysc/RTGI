#include "Voxelizer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Voxelizer::Voxelizer(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
{
}
//----------------------------------------------------------------------------
Voxelizer::~Voxelizer()
{
}
//----------------------------------------------------------------------------
void Voxelizer::Initialize(int voxelGridDim, int voxelGridLocalGroupDim)
{
    mVoxelGridDim = voxelGridDim;
    mVoxelGridLocalGroupDim = voxelGridLocalGroupDim;
}
//----------------------------------------------------------------------------
int Voxelizer::GetVoxelGridDim() const
{
    return mVoxelGridDim;
}
//----------------------------------------------------------------------------
int Voxelizer::GetVoxelGridLocalGroupDim() const
{
    return mVoxelGridLocalGroupDim;
}
//----------------------------------------------------------------------------