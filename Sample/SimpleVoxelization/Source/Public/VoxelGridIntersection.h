#ifndef RTGI_VoxelGridIntersection_H
#define RTGI_VoxelGridIntersection_H

#include "ComputeTask.h"
#include "AABB.h"
#include "ShaderUniform.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/24/2014
//----------------------------------------------------------------------------
class VoxelGridIntersection : public ComputeTask
{
public:
    VoxelGridIntersection();
    ~VoxelGridIntersection();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

    AABB* SceneBB;

private:
    ShaderUniform mSceneBBCenterLoc;
    ShaderUniform mSceneBBExtensionLoc;
    ShaderUniform mDimLoc;
    ShaderUniform mRayStartPointLoc;
    ShaderUniform mRayEndPointLoc;
};

typedef RefPointer<VoxelGridIntersection> VoxelGridIntersectionPtr;

}

#endif