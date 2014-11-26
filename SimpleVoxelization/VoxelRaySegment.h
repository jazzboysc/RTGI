#ifndef RTGI_VoxelRaySegment_H
#define RTGI_VoxelRaySegment_H

#include "PolylineGeometry.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/23/2014
//----------------------------------------------------------------------------
class VoxelRaySegment : public PolylineGeometry
{
public:
    VoxelRaySegment(Material* material, Camera* camera);
    virtual ~VoxelRaySegment();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnUpdateShaderConstants(int technique, int pass);

    vec3 MaterialColor;

private:
    GLint mMaterialColorLoc;
};

typedef RefPointer<VoxelRaySegment> VoxelRaySegmentPtr;

}

#endif