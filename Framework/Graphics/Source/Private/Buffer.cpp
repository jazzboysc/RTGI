//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Buffer.h"

using namespace RTGI;

GLenum Buffer::msBufferUsage[BufferUsage_Max] =
{
    GL_STATIC_READ,
    GL_STATIC_COPY,
    GL_STATIC_DRAW,
    GL_DYNAMIC_READ,
    GL_DYNAMIC_COPY,
    GL_DYNAMIC_DRAW
};

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
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
	return data;
}
//----------------------------------------------------------------------------
void Buffer::Unmap()
{
	glUnmapBuffer(mType);
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void Buffer::Bind(GLuint index)
{
#ifndef __APPLE__

	glBindBufferBase(mType, index, mBuffer);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert( res == GL_NO_ERROR );
#endif

#else
    assert( false );
#endif
}
//----------------------------------------------------------------------------
void Buffer::Bind()
{
	glBindBuffer(mType, mBuffer);
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void Buffer::BindToIndirect()
{
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, mBuffer);
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void Buffer::UpdateSubData(GLuint bindingPoint, int offset, size_t size, 
	void* data)
{
	Bind(bindingPoint);
	glBufferSubData(mType, offset, size, data);
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
bool Buffer::LoadFromSystemMemory(size_t size, void* data, BufferUsage usage)
{
	mSize = size;
	glGenBuffers(1, &mBuffer);
	glBindBuffer(mType, mBuffer);
    glBufferData(mType, size, data, msBufferUsage[(int)usage]);
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

	return true;
}
//----------------------------------------------------------------------------
void Buffer::ReserveMutableDeviceResource(size_t size, BufferUsage usage)
{
	mSize = size;
	glGenBuffers(1, &mBuffer);
	glBindBuffer(mType, mBuffer);
    glBufferData(mType, size, 0, msBufferUsage[(int)usage]);
	glBindBuffer(mType, 0);
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------