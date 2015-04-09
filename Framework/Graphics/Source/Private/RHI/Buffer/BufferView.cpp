//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "BufferView.h"

using namespace RTGI;

//----------------------------------------------------------------------------
BufferView::BufferView(const BufferViewDesc& viewDesc)
    :
    mViewDesc(viewDesc),
    mBufferViewHandle(0)
{
}
//----------------------------------------------------------------------------
BufferView::~BufferView()
{
    // TODO:
}
//----------------------------------------------------------------------------
void BufferView::CreateDeviceResource(GPUDevice* device)
{
    // TODO:
    if( !mBufferViewHandle )
    {
        // Create device resource.
    }
}
//----------------------------------------------------------------------------