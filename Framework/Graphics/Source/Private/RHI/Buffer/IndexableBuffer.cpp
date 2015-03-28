//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "IndexableBuffer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
IndexableBuffer::IndexableBuffer(BufferType type)
    :
    Buffer(type)
{
}
//----------------------------------------------------------------------------
IndexableBuffer::IndexableBuffer()
    :
    Buffer(BT_Unknown)
{
}
//----------------------------------------------------------------------------
IndexableBuffer::~IndexableBuffer()
{
}
//----------------------------------------------------------------------------
void IndexableBuffer::Bind(unsigned int index)
{
    RTGI_ASSERT(mBufferHandle);
    mBufferHandle->Device->BufferBindIndex(this, index);
}
//----------------------------------------------------------------------------
void IndexableBuffer::UpdateSubData(unsigned int index, int offset, 
    size_t size, void* data)
{
    RTGI_ASSERT(mBufferHandle);
    Bind(index);
    mBufferHandle->Device->BufferUpdateSubData(this, offset,
        size, data);
}
//----------------------------------------------------------------------------