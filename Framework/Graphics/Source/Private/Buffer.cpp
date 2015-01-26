//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Buffer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Buffer::Buffer(BufferType type)
	:
	mBufferHandle(0),
	mSize(0),
	mType(type)
{
}
//----------------------------------------------------------------------------
Buffer::~Buffer()
{
    GPU_DEVICE_FUNC(mBufferHandle->Device, DeleteBuffer)(this);
}
//----------------------------------------------------------------------------
void* Buffer::Map(BufferAccess access)
{
    assert(mBufferHandle);
    return GPU_DEVICE_FUNC(mBufferHandle->Device, BufferMap)(this, access);
}
//----------------------------------------------------------------------------
void Buffer::Unmap()
{
    assert(mBufferHandle);
    GPU_DEVICE_FUNC(mBufferHandle->Device, BufferUnmap)(this);
}
//----------------------------------------------------------------------------
void Buffer::Bind(unsigned int index)
{
    assert(mBufferHandle);
    GPU_DEVICE_FUNC(mBufferHandle->Device, BufferBindIndex)(this, index);
}
//----------------------------------------------------------------------------
void Buffer::Bind()
{
    assert(mBufferHandle);
    GPU_DEVICE_FUNC(mBufferHandle->Device, BufferBind)(this);
}
//----------------------------------------------------------------------------
void Buffer::BindToIndirect()
{
    assert(mBufferHandle);
    GPU_DEVICE_FUNC(mBufferHandle->Device, BufferBindToIndirect)(this);
}
//----------------------------------------------------------------------------
void Buffer::UpdateSubData(unsigned int bindingPoint, int offset, size_t size, 
	void* data)
{
    assert(mBufferHandle);
	Bind(bindingPoint);
    GPU_DEVICE_FUNC(mBufferHandle->Device, BufferUpdateSubData)(this, offset, 
        size, data);
}
//----------------------------------------------------------------------------
bool Buffer::LoadFromSystemMemory(GPUDevice* device, size_t size, void* data, 
    BufferUsage usage)
{
	mSize = size;
    mBufferHandle = GPU_DEVICE_FUNC(device, BufferLoadFromSystemMemory)(this, 
        size, data, usage);

	return true;
}
//----------------------------------------------------------------------------
void Buffer::ReserveMutableDeviceResource(GPUDevice* device, size_t size, 
    BufferUsage usage)
{
	mSize = size;
    mBufferHandle = GPU_DEVICE_FUNC(device, BufferLoadFromSystemMemory)(this, 
        size, 0, usage);
}
//----------------------------------------------------------------------------
void Buffer::ReserveImmutableDeviceResource(GPUDevice* device, size_t size)
{
    mSize = size;
    mBufferHandle = GPU_DEVICE_FUNC(device, 
        BufferLoadImmutableFromSystemMemory)(this, size, 0);
}
//----------------------------------------------------------------------------
void Buffer::Clear(BufferInternalFormat internalFormat, BufferFormat format,
    BufferComponentType type, void* data)
{
    assert(mBufferHandle);
    GPU_DEVICE_FUNC(mBufferHandle->Device, BufferClear)(this, internalFormat, 
        format, type, data);
}
//----------------------------------------------------------------------------