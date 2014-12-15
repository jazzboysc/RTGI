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
void Voxelizer::Initialize()
{

}
//----------------------------------------------------------------------------
int Voxelizer::GetVoxelGridDim() const
{
    return VOXEL_DIMENSION;
}
//----------------------------------------------------------------------------
int Voxelizer::GetVoxelGridLocalGroupDim() const
{
    return VOXEL_LOCAL_GROUP_DIM;
}
//----------------------------------------------------------------------------