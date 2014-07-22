//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "PixelBuffer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
PixelBuffer::PixelBuffer()
	:
	Buffer(GL_PIXEL_UNPACK_BUFFER)
{
}
//----------------------------------------------------------------------------
PixelBuffer::~PixelBuffer()
{
}
//----------------------------------------------------------------------------