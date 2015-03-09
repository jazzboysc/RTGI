//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Primitive_H
#define RTGI_Primitive_H

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 03/09/2015
//----------------------------------------------------------------------------
class Primitive
{
public:
    Primitive();
    ~Primitive();

    VertexBufferPtr VB;
    IndexBufferPtr IB;
};

#include "Primitive.inl"

}

#endif