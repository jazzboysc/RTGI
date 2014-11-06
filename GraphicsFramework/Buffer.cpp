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
void Buffer::Bind(GLuint index)
{
#ifndef __APPLE__
	glBindBufferBase(mType, index, mBuffer);
#else
    assert( false );
#endif
}
//----------------------------------------------------------------------------
void Buffer::Bind()
{
	glBindBuffer(mType, mBuffer);
}
//----------------------------------------------------------------------------
void Buffer::BindToIndirect()
{
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mBuffer);
}
//----------------------------------------------------------------------------
void Buffer::UpdateSubData(GLuint bindingPoint, int offset, size_t size, 
	void* data)
{
	Bind(bindingPoint);
	glBufferSubData(mType, offset, size, data);
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
void Buffer::ReserveDeviceResource(GLuint size, GLenum usage)
{
	mSize = size;
	glGenBuffers(1, &mBuffer);
	glBindBuffer(mType, mBuffer);
	glBufferData(mType, size, 0, usage);
	glBindBuffer(mType, 0);
}
//----------------------------------------------------------------------------