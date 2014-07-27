//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "ComputingFrameworkPCH.h"
#include "MemoryObject.h"

using namespace RTGI;

//----------------------------------------------------------------------------
MemoryObject::MemoryObject()
{
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
void MemoryObject::CreateFromSystemMemory(cl_context context, cl_mem_flags flags,
    size_t size, void* data)
{
    cl_int res;
    mMO = clCreateBuffer(context, flags, size, data, &res);
    assert( res >= 0 );
}
//----------------------------------------------------------------------------
void MemoryObject::CreateFromTexture2D(cl_context context, cl_mem_flags flags,
    Texture2D* texture)
{
    assert( texture );
    
    cl_int res;
    mMO = clCreateFromGLTexture(context, flags, GL_TEXTURE_2D, 0,
        texture->GetTexture(), &res);
    assert( res >= 0 );
}
//----------------------------------------------------------------------------