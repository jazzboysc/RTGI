//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "DispatchIndirectBuffer.h"

#ifndef __APPLE__

using namespace RTGI;

//----------------------------------------------------------------------------
DispatchIndirectBuffer::DispatchIndirectBuffer()
    :
    Buffer(GL_DISPATCH_INDIRECT_BUFFER)
{
}
//----------------------------------------------------------------------------
DispatchIndirectBuffer::~DispatchIndirectBuffer()
{
}
//----------------------------------------------------------------------------

#endif