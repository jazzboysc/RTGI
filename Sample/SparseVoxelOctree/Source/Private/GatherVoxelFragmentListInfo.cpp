#include "GatherVoxelFragmentListInfo.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GatherVoxelFragmentListInfo::GatherVoxelFragmentListInfo()
{
}
//----------------------------------------------------------------------------
GatherVoxelFragmentListInfo::~GatherVoxelFragmentListInfo()
{
}
//----------------------------------------------------------------------------
void GatherVoxelFragmentListInfo::OnGetShaderConstants()
{
}
//----------------------------------------------------------------------------
void GatherVoxelFragmentListInfo::OnPreDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
//----------------------------------------------------------------------------
void GatherVoxelFragmentListInfo::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
}
//----------------------------------------------------------------------------