//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_IndexableBuffer_H
#define RTGI_IndexableBuffer_H

#include "Buffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/30/2015
//----------------------------------------------------------------------------
class IndexableBuffer : public Buffer
{
public:
    IndexableBuffer(BufferType type);
    ~IndexableBuffer();

    using Buffer::Bind;
    void Bind(unsigned int index);

    using Buffer::BindTo;
    void BindTo(unsigned int index, BufferView* view);

    void UpdateSubData(unsigned int index, int offset, size_t size,
        void* data);

private:
    IndexableBuffer();
};

typedef RefPointer<IndexableBuffer> IndexableBufferBufferPtr;

}


#endif