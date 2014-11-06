#include "GatherVoxelBuffer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GatherVoxelBuffer::GatherVoxelBuffer()
{
}
//----------------------------------------------------------------------------
GatherVoxelBuffer::~GatherVoxelBuffer()
{
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnPreDispatch(unsigned int pass)
{
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
}
//----------------------------------------------------------------------------