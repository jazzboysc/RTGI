//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_IndexBuffer_H
#define RTGI_IndexBuffer_H

#include "Buffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 03/09/2015
//----------------------------------------------------------------------------
class IndexBuffer : public Buffer
{
public:
    IndexBuffer();
    ~IndexBuffer();
};

typedef RefPointer<IndexBuffer> IndexBufferPtr;

}

#endif