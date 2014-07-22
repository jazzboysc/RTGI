//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Buffer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Buffer::Buffer(GLenum type)
	:
	mBuffer(0),
	mSize(0),
	mType(type)
{
}
//----------------------------------------------------------------------------
Buffer::~Buffer()
{
	glDeleteBuffers(1, &mBuffer);
}
//----------------------------------------------------------------------------
GLuint Buffer::GetBuffer() const
{
	return mBuffer;
}
//----------------------------------------------------------------------------
GLuint Buffer::GetSize() const
{
	return mSize;
}
//----------------------------------------------------------------------------
void* Buffer::Map(GLenum access)
{
	void* data = glMapBuffer(mType, access);
	return data;
}
//----------------------------------------------------------------------------
void Buffer::Unmap()
{
	glUnmapBuffer(mType);
}
//----------------------------------------------------------------------------
bool Buffer::LoadFromSystemMemory(GLuint size, void* data, GLenum usage)
{
	mSize = size;
	glGenBuffers(1, &mBuffer);
	glBindBuffer(mType, mBuffer);
	glBufferData(mType, size, data, usage);

	return true;
}
//----------------------------------------------------------------------------