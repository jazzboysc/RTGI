//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "BufferView.h"
#include "BufferBase.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BufferView::BufferView(const BufferViewDesc& viewDesc)
    :
    mViewDesc(viewDesc),
    mBufferViewHandle(0),
    mBufferBase(0)
{
}
//----------------------------------------------------------------------------
BufferView::~BufferView()
{
    if( mBufferViewHandle )
    {
        mBufferViewHandle->Device->DeleteBufferView(this);
        delete mBufferViewHandle;
    }
}
//----------------------------------------------------------------------------
void BufferView::CreateDeviceResource(GPUDevice* device, 
    BufferBase* bufferBase)
{
    if( !mBufferViewHandle )
    {
        mBufferBase = bufferBase;
        mBufferViewHandle = device->CreateBufferView(this);
    }
}
//----------------------------------------------------------------------------