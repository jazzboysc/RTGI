//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "TextureBuffer.h"

#ifndef __APPLE__

using namespace RTGI;

//----------------------------------------------------------------------------
TextureBuffer::TextureBuffer()
	:
	Buffer(BT_Texture)
{
}
//----------------------------------------------------------------------------
TextureBuffer::~TextureBuffer()
{
}
//----------------------------------------------------------------------------

#endif