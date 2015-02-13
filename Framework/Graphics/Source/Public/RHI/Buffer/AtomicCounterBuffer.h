//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_AtomicCounterBuffer_H
#define RTGI_AtomicCounterBuffer_H

#ifndef __APPLE__

#include "IndexableBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/21/2013
//----------------------------------------------------------------------------
class AtomicCounterBuffer : public IndexableBuffer
{
public:
	AtomicCounterBuffer();
	~AtomicCounterBuffer();
};

typedef RefPointer<AtomicCounterBuffer> AtomicCounterBufferPtr;

}

#endif

#endif