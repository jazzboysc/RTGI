//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "StructuredBuffer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
StructuredBuffer::StructuredBuffer()
	:
	Buffer(GL_SHADER_STORAGE_BUFFER)
{
}
//----------------------------------------------------------------------------
StructuredBuffer::~StructuredBuffer()
{
}
//----------------------------------------------------------------------------