//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PixelBuffer_H
#define RTGI_PixelBuffer_H

#include "Buffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/21/2013
//----------------------------------------------------------------------------
class PixelBuffer : public Buffer
{
public:
	PixelBuffer();
	~PixelBuffer();
};

typedef RefPointer<PixelBuffer> PixelBufferPtr;

}

#endif