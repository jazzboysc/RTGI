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
    // TODO:
}
//----------------------------------------------------------------------------
void BufferView::CreateDeviceResource(GPUDevice* device, 
    BufferBase* bufferBase)
{
    // TODO:
    if( !mBufferViewHandle )
    {
        // Create device resource.
        mBufferBase = bufferBase;
    }
}
//----------------------------------------------------------------------------