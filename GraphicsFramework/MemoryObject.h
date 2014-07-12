//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_MemoryObject_H
#define RTGI_MemoryObject_H

#include "FrameworkCommon.h"
#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/11/2014
//----------------------------------------------------------------------------
class MemoryObject : public RefObject
{
public:
	MemoryObject(cl_context context, cl_mem_flags flags, size_t size,
        void* data);
	~MemoryObject();

	cl_mem GetMO();

private:
	cl_mem mMO;
};

typedef RefPointer<MemoryObject> MemoryObjectPtr;

}

#endif