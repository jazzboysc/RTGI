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
    mSize(0)
{
    BufferViewDesc viewDesc;
    viewDesc.Type = type;
    mDefaultView = new BufferView(viewDesc);
}
//----------------------------------------------------------------------------
Buffer::Buffer(BufferView* defaultView)
	:
	mBufferHandle(0),
	mSize(0),
    mDefaultView(defaultView)
{
}
//----------------------------------------------------------------------------
Buffer::~Buffer()
{
	mBufferHandle->Device->DeleteBuffer(this);
    mDefaultView = 0;
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
void Buffer::BindTo(BufferView* view)
{
    RTGI_ASSERT(mBufferHandle && view);
    mBufferHandle->Device->BufferBindTo(this, view);
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
    mDefaultView->CreateDeviceResource(device);

	mSize = size;
    mBufferHandle = device->BufferLoadFromSystemMemory(this, 
        size, data, usage);

	return true;
}
//----------------------------------------------------------------------------
void Buffer::ReserveMutableDeviceResource(GPUDevice* device, size_t size, 
    BufferUsage usage)
{
    mDefaultView->CreateDeviceResource(device);

	mSize = size;
    mBufferHandle = device->BufferLoadFromSystemMemory(this, 
        size, 0, usage);
}
//----------------------------------------------------------------------------
void Buffer::ReserveImmutableDeviceResource(GPUDevice* device, size_t size)
{
    mDefaultView->CreateDeviceResource(device);

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