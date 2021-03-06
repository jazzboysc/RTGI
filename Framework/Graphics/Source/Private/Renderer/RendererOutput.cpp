//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "RendererOutput.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RendererOutput::RendererOutput()
{
}
//----------------------------------------------------------------------------
RendererOutput::RendererOutput(const std::string& name, 
    BufferBase* outputBuffer, RendererOutputType outputType, BindingFlag flag,
    BufferType viewType, unsigned int binding, bool reset, int resetValue)
{
    Name = name;
    OutputBuffer = outputBuffer;
    OutputType = outputType;
    Flag = flag;
    Binding = binding;
    Reset = reset;
    ResetValue = resetValue;

    if( Flag == BF_BindTo || Flag == BF_BindIndexTo )
    {
        BufferViewDesc viewDesc;
        viewDesc.Type = viewType;
        OutputBufferView = new BufferView(viewDesc);
    }
}
//----------------------------------------------------------------------------
RendererOutput::~RendererOutput()
{
    OutputBuffer = 0;
    OutputBufferView = 0;
}
//----------------------------------------------------------------------------
void RendererOutput::Enable()
{
    RTGI_ASSERT(OutputType == ROT_Buffer);
    Buffer* buffer = (Buffer*)(BufferBase*)OutputBuffer;
    switch( Flag )
    {
    case BF_Bind:
        buffer->Bind();
        break;

    case BF_BindIndex:
        ((IndexableBuffer*)buffer)->Bind(Binding);
        break;

    case BF_BindTo:
        buffer->BindTo(OutputBufferView);
        break;

    case BF_BindIndexTo:
        ((IndexableBuffer*)buffer)->Bind(Binding);
        buffer->BindTo(OutputBufferView);
        break;

    default:
        RTGI_ASSERT(false);
        break;
    }

    if( Reset )
    {
        size_t bufferSize = buffer->GetSize();
        int* bufferData = (int*)buffer->Map(BA_Write_Only);
        RTGI_ASSERT(bufferData);
        memset(bufferData, ResetValue, bufferSize);
        buffer->Unmap();
    }
}
//----------------------------------------------------------------------------
void RendererOutput::Disable()
{
    // Nothing to do.
}
//----------------------------------------------------------------------------