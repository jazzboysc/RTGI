//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "DispatchIndirectBuffer.h"

#ifndef __APPLE__

using namespace RTGI;

//----------------------------------------------------------------------------
DispatchIndirectBuffer::DispatchIndirectBuffer()
    :
    Buffer(BT_DispatchIndirect)
{
}
//----------------------------------------------------------------------------
DispatchIndirectBuffer::~DispatchIndirectBuffer()
{
}
//----------------------------------------------------------------------------

#endif