#include "VoxelRaySegment.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VoxelRaySegment::VoxelRaySegment(Material* material, Camera* camera)
    :
    PolylineGeometry(material, camera)
{
}
//----------------------------------------------------------------------------
VoxelRaySegment::~VoxelRaySegment()
{
}
//----------------------------------------------------------------------------
void VoxelRaySegment::OnGetShaderConstants()
{
    PolylineGeometry::OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void VoxelRaySegment::OnUpdateShaderConstants(int technique, int pass)
{
    PolylineGeometry::OnUpdateShaderConstants(technique, pass);
}
//----------------------------------------------------------------------------