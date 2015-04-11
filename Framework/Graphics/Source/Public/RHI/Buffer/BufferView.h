//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_BufferView_H
#define RTGI_BufferView_H

#include "RefObject.h"
#include "GPUResource.h"

namespace RTGI
{

enum BufferType
{
    BT_Typeless = -1,
    BT_AtomicCounter,
    BT_DispatchIndirect,
    BT_DrawIndirect,
    BT_Pixel,
    BT_Structured,
    BT_TextureBuffer,
    BT_Uniform,
    BT_Vertex,
    BT_Index,
    BufferType_Max
};

struct BufferViewDesc
{
    BufferType Type;
};

class BufferBase;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 04/09/2015
//----------------------------------------------------------------------------
class BufferView : public RefObject
{
public:
    BufferView(const BufferViewDesc& viewDesc);
    ~BufferView();

    inline BufferType GetBufferType() const { return mViewDesc.Type; }

    void CreateDeviceResource(GPUDevice* device, BufferBase* bufferBase);

private:
    BufferViewDesc mViewDesc;
    BufferViewHandle* mBufferViewHandle;
    BufferBase* mBufferBase;
};

typedef RefPointer<BufferView> BufferViewPtr;

}

#endif