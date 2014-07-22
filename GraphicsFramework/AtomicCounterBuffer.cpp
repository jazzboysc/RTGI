//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "AtomicCounterBuffer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
AtomicCounterBuffer::AtomicCounterBuffer()
	:
	Buffer(GL_ATOMIC_COUNTER_BUFFER)
{
}
//----------------------------------------------------------------------------
AtomicCounterBuffer::~AtomicCounterBuffer()
{
}
//----------------------------------------------------------------------------