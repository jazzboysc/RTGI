#include "VoxelRaySegment.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VoxelRaySegment::VoxelRaySegment(Material* material, Camera* camera)
    :
    PolylineGeometry(material, camera),
    MaterialColor(0.75f, 0.75f, 0.75f)
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