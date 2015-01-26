//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Buffer_H
#define RTGI_Buffer_H

#include "BufferBase.h"

namespace RTGI
{

enum BufferType
{
    BT_Unknown = -1,
    BT_AtomicCounter,
    BT_DispatchIndirect,
    BT_Pixel,
    BT_Structured,
    BT_Texture,
    BT_Uniform,
    BufferType_Max
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/21/2013
//----------------------------------------------------------------------------
class Buffer : public BufferBase
{
public:
    Buffer(BufferType type);
	virtual ~Buffer();

    void* Map(BufferAccess access);
	void Unmap();

	void Bind(unsigned int index);
	void Bind();
    void BindToIndirect();

	void UpdateSubData(unsigned int bindingPoint, int offset, size_t size, 
        void* data);

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
    inline BufferType GetType() const { return mType; }

protected:
    BufferHandle* mBufferHandle;
    size_t mSize;
    BufferType mType;
};

typedef RefPointer<Buffer> BufferPtr;

}

#endif