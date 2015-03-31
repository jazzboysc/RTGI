//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Primitive_H
#define RTGI_Primitive_H

#include "RefObject.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 03/09/2015
//----------------------------------------------------------------------------
class Primitive : public RefObject
{
public:
    Primitive();
    ~Primitive();

    VertexBufferPtr VB;
    IndexBufferPtr IB;
};

typedef RefPointer<Primitive> PrimitivePtr;

#include "Primitive.inl"

}

#endif