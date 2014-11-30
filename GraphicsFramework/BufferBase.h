//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_BufferBase_H
#define RTGI_BufferBase_H

#include "FrameworkCommon.h"
#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2013
//----------------------------------------------------------------------------
class BufferBase : public RefObject
{
public:
    BufferBase();
    virtual ~BufferBase();
};

typedef RefPointer<BufferBase> BufferBasePtr;

}

#endif