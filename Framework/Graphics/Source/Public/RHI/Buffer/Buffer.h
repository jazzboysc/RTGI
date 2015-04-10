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
    Buffer(BufferType type);
    Buffer(BufferView* defaultView);
	virtual ~Buffer();

    void* Map(BufferAccess access);
	void Unmap();

	void Bind();
    void BindTo(BufferView* view);
    void BindToIndirect();

	// Load buffer data from system memory. User is responsible for deleting
	// the system memory data.
    bool LoadFromSystemMemory(GPUDevice* device, size_t size, void* data, 
        BufferUsage usage);

    void ReserveMutableDeviceResource(GPUDevice* device, size_t size, 
        BufferUsage usage);

    void ReserveImmutableDeviceResource(GPUDevice* device, size_t size);

    void Clear(BufferInternalFormat internalFormat, BufferFormat format, 
        BufferComponentType type, void* data);

    inline BufferHandle* GetBufferHandle() const { return mBufferHandle; }
    inline size_t GetSize() const { return mSize; }
    inline BufferType GetType() const { return mDefaultView->GetBufferType(); }

protected:
    BufferHandle* mBufferHandle;
    size_t mSize;
    BufferViewPtr mDefaultView;
};

typedef RefPointer<Buffer> BufferPtr;

}

#endif