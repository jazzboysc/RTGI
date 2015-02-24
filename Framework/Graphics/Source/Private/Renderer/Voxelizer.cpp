//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Voxelizer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Voxelizer::Voxelizer(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
{
    RasterizerDimBias = 0;
    VoxelGridDim = 0;
}
//----------------------------------------------------------------------------
Voxelizer::~Voxelizer()
{
}
//----------------------------------------------------------------------------