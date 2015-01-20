//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_StructuredBuffer_H
#define RTGI_StructuredBuffer_H

#ifndef __APPLE__

#include "Buffer.h"

namespace RTGI
{

typedef struct _DrawElementsIndirectCommand
{
    unsigned int Count;
    unsigned int PrimCount;
    unsigned int FirstIndex;
    unsigned int BaseVertex;
    unsigned int BaseInstance;
} DrawElementsIndirectCommand;

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

#endif