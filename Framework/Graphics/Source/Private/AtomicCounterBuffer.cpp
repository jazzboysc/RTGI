//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "AtomicCounterBuffer.h"

#ifndef __APPLE__

using namespace RTGI;

//----------------------------------------------------------------------------
AtomicCounterBuffer::AtomicCounterBuffer()
	:
    Buffer(BT_AtomicCounter)
{
}
//----------------------------------------------------------------------------
AtomicCounterBuffer::~AtomicCounterBuffer()
{
}
//----------------------------------------------------------------------------

#endif