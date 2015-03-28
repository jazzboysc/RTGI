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
	mBufferHandle->Device->DeleteBuffer(this);
}
//----------------------------------------------------------------------------
void* Buffer::Map(BufferAccess access)
{
    RTGI_ASSERT(mBufferHandle);
    return mBufferHandle->Device->BufferMap(this, access);
}
//----------------------------------------------------------------------------
void Buffer::Unmap()
{
    RTGI_ASSERT(mBufferHandle);
	mBufferHandle->Device->BufferUnmap(this);
}
//----------------------------------------------------------------------------
void Buffer::Bind()
{
    RTGI_ASSERT(mBufferHandle);
	mBufferHandle->Device->BufferBind(this);
}
//----------------------------------------------------------------------------
void Buffer::BindToIndirect()
{
    RTGI_ASSERT(mBufferHandle);
	mBufferHandle->Device->BufferBindToIndirect(this);
}
//----------------------------------------------------------------------------
bool Buffer::LoadFromSystemMemory(GPUDevice* device, size_t size, void* data, 
    BufferUsage usage)
{
	mSize = size;
    mBufferHandle = device->BufferLoadFromSystemMemory(this, 
        size, data, usage);

	return true;
}
//----------------------------------------------------------------------------
void Buffer::ReserveMutableDeviceResource(GPUDevice* device, size_t size, 
    BufferUsage usage)
{
	mSize = size;
    mBufferHandle = device->BufferLoadFromSystemMemory(this, 
        size, 0, usage);
}
//----------------------------------------------------------------------------
void Buffer::ReserveImmutableDeviceResource(GPUDevice* device, size_t size)
{
    mSize = size;
    mBufferHandle = device->BufferLoadImmutableFromSystemMemory(this, size, 0);
}
//----------------------------------------------------------------------------
void Buffer::Clear(BufferInternalFormat internalFormat, BufferFormat format,
    BufferComponentType type, void* data)
{
    RTGI_ASSERT(mBufferHandle);
    mBufferHandle->Device->BufferClear(this, internalFormat, 
        format, type, data);
}
//----------------------------------------------------------------------------