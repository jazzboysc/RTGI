//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Buffer.h"

using namespace RTGI;

GLenum gsBufferFormat[BufferFormat_Max] =
{
    GL_R,
    GL_RG,
    GL_RGB,
    GL_RGBA,
    GL_RGB,
    GL_RGBA,
    GL_RED_INTEGER,
    GL_RED,
    GL_DEPTH_COMPONENT
};

GLint gsBufferInternalFormat[BufferInternalFormat_Max] =
{
    GL_RGB8,
    GL_RGBA8,
    GL_RGB32F_ARB,
    GL_RGBA32F_ARB,
    GL_RGBA32UI,
    GL_RGB16F_ARB,
    GL_RGBA16F_ARB,
    GL_R32UI,
    GL_R32F,
    GL_RG32F,
    GL_DEPTH_COMPONENT24
};

GLenum gsBufferComponentType[BufferComponentType_Max] =
{
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_INT,
    GL_FLOAT
};

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
void Buffer::ReserveImmutableDeviceResource(size_t size)
{
    mSize = size;
    glGenBuffers(1, &mBuffer);
    glBindBuffer(mType, mBuffer);
    glBufferStorage(mType, size, 0, 0);
    glBindBuffer(mType, 0);
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void Buffer::Clear(BufferInternalFormat internalFormat, BufferFormat format,
    BufferComponentType type, void* data)
{
    glInvalidateBufferData(mBuffer);
    glClearBufferData(mType, gsBufferInternalFormat[(int)internalFormat],
        gsBufferFormat[(int)format], gsBufferComponentType[(int)type], data);
#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------