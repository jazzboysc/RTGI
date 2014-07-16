//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_MemoryObject_H
#define RTGI_MemoryObject_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "Texture2D.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/11/2014
//----------------------------------------------------------------------------
class MemoryObject : public RefObject
{
public:
	MemoryObject();
	~MemoryObject();
    
    void CreateFromSystemMemory(cl_context context, cl_mem_flags flags,
        size_t size, void* data);
    
    void CreateFromTexture2D(cl_context context, cl_mem_flags flags,
        Texture2D* texture);

	cl_mem GetMO();

private:
	cl_mem mMO;
};

typedef RefPointer<MemoryObject> MemoryObjectPtr;

}

#endif