#ifndef RTGI_ResetSVOBuffer_H
#define RTGI_ResetSVOBuffer_H

#include "ComputeTask.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/28/2015
//----------------------------------------------------------------------------
class ResetSVOBuffer : public ComputeTask
{
public:
    ResetSVOBuffer();
    ~ResetSVOBuffer();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);
};

typedef RefPointer<ResetSVOBuffer> ResetSVOBufferPtr;

}

#endif