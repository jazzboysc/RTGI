//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "StructuredBuffer.h"

#ifndef __APPLE__

using namespace RTGI;

//----------------------------------------------------------------------------
StructuredBuffer::StructuredBuffer()
	:
	Buffer(BT_Structured)
{
}
//----------------------------------------------------------------------------
StructuredBuffer::~StructuredBuffer()
{
}
//----------------------------------------------------------------------------

#endif