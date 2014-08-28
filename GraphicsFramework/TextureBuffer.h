//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_TextureBuffer_H
#define RTGI_TextureBuffer_H

#ifndef __APPLE__

#include "FrameworkCommon.h"
#include "Buffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/21/2013
//----------------------------------------------------------------------------
class TextureBuffer : public Buffer
{
public:
	TextureBuffer();
	~TextureBuffer();
};

typedef RefPointer<TextureBuffer> TextureBufferPtr;

}

#endif

#endif