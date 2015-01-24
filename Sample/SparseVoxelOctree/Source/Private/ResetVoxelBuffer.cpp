#include "ResetVoxelBuffer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ResetVoxelBuffer::ResetVoxelBuffer()
{
}
//----------------------------------------------------------------------------
ResetVoxelBuffer::~ResetVoxelBuffer()
{
}
//----------------------------------------------------------------------------
void ResetVoxelBuffer::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void ResetVoxelBuffer::OnPreDispatch(unsigned int pass)
{
}
//----------------------------------------------------------------------------
void ResetVoxelBuffer::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
//----------------------------------------------------------------------------