//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Buffer_H
#define RTGI_Buffer_H


#include "BufferBase.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/21/2013
//----------------------------------------------------------------------------
class Buffer : public BufferBase
{
public:
	Buffer(GLenum type);
	virtual ~Buffer();

	void* Map(GLenum access);
	void Unmap();
	void Bind(GLuint index);
	void Bind();
    void BindToIndirect();
	void UpdateSubData(GLuint bindingPoint, int offset, size_t size, void* data);

	// Load buffer data from system memory. User is responsible for deleting
	// the system memory data.
    bool LoadFromSystemMemory(size_t size, void* data, BufferUsage usage);

    void ReserveDeviceResource(size_t size, BufferUsage usage);

	GLuint GetBuffer() const;
	GLuint GetSize() const;

protected:
	GLuint mBuffer;
	GLuint mSize;
	GLenum mType;

    static GLenum msBufferUsage[BU_Max];
};

typedef RefPointer<Buffer> BufferPtr;

}

#endif