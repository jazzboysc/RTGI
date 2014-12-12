//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_UniformBuffer_H
#define RTGI_UniformBuffer_H


#include "Buffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/23/2014
//----------------------------------------------------------------------------
class UniformBuffer : public Buffer
{
public:
    UniformBuffer();
    ~UniformBuffer();
};

typedef RefPointer<UniformBuffer> UniformBufferPtr;

}

#endif