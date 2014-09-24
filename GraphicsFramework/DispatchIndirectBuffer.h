//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_DispatchIndirectBuffer_H
#define RTGI_DispatchIndirectBuffer_H

#ifndef __APPLE__

#include "FrameworkCommon.h"
#include "Buffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/23/2014
//----------------------------------------------------------------------------
class DispatchIndirectBuffer : public Buffer
{
public:
    DispatchIndirectBuffer();
    ~DispatchIndirectBuffer();
};

typedef RefPointer<DispatchIndirectBuffer> DispatchIndirectBufferPtr;

}

#endif

#endif