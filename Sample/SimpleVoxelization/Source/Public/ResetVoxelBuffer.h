#ifndef RTGI_ResetVoxelBuffer_H
#define RTGI_ResetVoxelBuffer_H

#include "ComputeTask.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/05/2014
//----------------------------------------------------------------------------
class ResetVoxelBuffer : public ComputeTask
{
public:
    ResetVoxelBuffer();
    ~ResetVoxelBuffer();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);
};

typedef RefPointer<ResetVoxelBuffer> ResetVoxelBufferPtr;

}

#endif