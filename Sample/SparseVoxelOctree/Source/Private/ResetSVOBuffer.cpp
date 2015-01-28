#include "ResetSVOBuffer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ResetSVOBuffer::ResetSVOBuffer()
{
}
//----------------------------------------------------------------------------
ResetSVOBuffer::~ResetSVOBuffer()
{
}
//----------------------------------------------------------------------------
void ResetSVOBuffer::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void ResetSVOBuffer::OnPreDispatch(unsigned int pass)
{
}
//----------------------------------------------------------------------------
void ResetSVOBuffer::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}
//----------------------------------------------------------------------------