#ifndef RTGI_GatherVoxelFragmentListInfo_H
#define RTGI_GatherVoxelFragmentListInfo_H

#include "ComputeTask.h"
#include "AABB.h"
#include "ShaderUniform.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/26/2015
//----------------------------------------------------------------------------
class GatherVoxelFragmentListInfo : public ComputeTask
{
public:
    GatherVoxelFragmentListInfo();
    ~GatherVoxelFragmentListInfo();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

};

typedef RefPointer<GatherVoxelFragmentListInfo> GatherVoxelFragmentListInfoPtr;

}

#endif