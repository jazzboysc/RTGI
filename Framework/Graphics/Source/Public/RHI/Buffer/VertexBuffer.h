//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_VertexBuffer_H
#define RTGI_VertexBuffer_H

#include "Buffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 03/09/2015
//----------------------------------------------------------------------------
class VertexBuffer : public Buffer
{
public:
    VertexBuffer();
    ~VertexBuffer();
};

typedef RefPointer<VertexBuffer> VertexBufferPtr;

}

#endif