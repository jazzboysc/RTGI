#ifndef RTGI_GatherVoxelBuffer_H
#define RTGI_GatherVoxelBuffer_H

#include "ComputeTask.h"
#include "AABB.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/06/2014
//----------------------------------------------------------------------------
class GatherVoxelBuffer : public ComputeTask
{
public:
    GatherVoxelBuffer();
    ~GatherVoxelBuffer();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

    AABB* SceneBB;

private:
    GLint mSceneBBMinLoc;
    GLint mSceneBBExtensionLoc;
};

typedef RefPointer<GatherVoxelBuffer> GatherVoxelBufferPtr;

}

#endif