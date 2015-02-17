#include "SVORaySegment.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SVORaySegment::SVORaySegment(Material* material, Camera* camera)
    :
    PolylineGeometry(material, camera)
{
}
//----------------------------------------------------------------------------
SVORaySegment::~SVORaySegment()
{
}
//----------------------------------------------------------------------------
void SVORaySegment::OnGetShaderConstants()
{
    PolylineGeometry::OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void SVORaySegment::OnUpdateShaderConstants(int technique, int pass)
{
    PolylineGeometry::OnUpdateShaderConstants(technique, pass);
}
//----------------------------------------------------------------------------