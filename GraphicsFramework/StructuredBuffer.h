//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_StructuredBuffer_H
#define RTGI_StructuredBuffer_H

#include "FrameworkCommon.h"
#include "Buffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/22/2013
//----------------------------------------------------------------------------
class StructuredBuffer : public Buffer
{
public:
	StructuredBuffer();
	~StructuredBuffer();
};

typedef RefPointer<StructuredBuffer> StructuredBufferPtr;

}

#endif