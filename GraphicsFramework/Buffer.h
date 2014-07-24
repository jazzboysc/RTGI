//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Buffer_H
#define RTGI_Buffer_H

#include "FrameworkCommon.h"
#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/21/2013
//----------------------------------------------------------------------------
class Buffer : public RefObject
{
public:
	Buffer(GLenum type);
	virtual ~Buffer();

	void* Map(GLenum access);
	void Unmap();
	void Bind(GLuint bindingPoint);
	void UpdateSubData(GLuint bindingPoint, int offset, size_t size, void* data);

	// Load buffer data from system memory. User is responsible for deleting
	// the system memory data.
	bool LoadFromSystemMemory(GLuint size, void* data, GLenum usage);

	void ReserveDeviceResource(GLuint size, GLenum usage);

	GLuint GetBuffer() const;
	GLuint GetSize() const;

protected:
	GLuint mBuffer;
	GLuint mSize;
	GLenum mType;
};

typedef RefPointer<Buffer> BufferPtr;

}

#endif