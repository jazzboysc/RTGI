//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "MemoryObject.h"

using namespace RTGI;

//----------------------------------------------------------------------------
MemoryObject::MemoryObject(cl_context context, cl_mem_flags flags,
    size_t size, void* data)
{
    cl_int res;
    mMO = clCreateBuffer(context, flags, size, data, &res);
    assert( res >= 0 );
}
//----------------------------------------------------------------------------
MemoryObject::~MemoryObject()
{
    clReleaseMemObject(mMO);
}
//----------------------------------------------------------------------------
cl_mem MemoryObject::GetMO()
{
    return mMO;
}
//----------------------------------------------------------------------------