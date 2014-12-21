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
    BufferBase* outputBuffer, bool isTexture, BindingFlag flag, 
    unsigned int binding, bool reset, int resetValue)
{
    Name = name;
    OutputBuffer = outputBuffer;
    IsTexture = isTexture;
    Flag = flag;
    Binding = binding;
    Reset = reset;
    ResetValue = resetValue;
}
//----------------------------------------------------------------------------
RendererOutput::~RendererOutput()
{
    OutputBuffer = 0;
}
//----------------------------------------------------------------------------
void RendererOutput::Enable()
{
    assert(!IsTexture);
    Buffer* buffer = (Buffer*)(BufferBase*)OutputBuffer;
    switch( Flag )
    {
    case BF_Bind:
        buffer->Bind();
        break;

    case BF_BindIndex:
        buffer->Bind(Binding);
        break;

    case BF_BindToIndirect:
        buffer->BindToIndirect();
        break;

    default:
        assert(false);
        break;
    }

    if( Reset )
    {
        int* bufferData = (int*)buffer->Map(GL_WRITE_ONLY);
        assert(bufferData);
        bufferData[0] = ResetValue;
        buffer->Unmap();
    }
}
//----------------------------------------------------------------------------
void RendererOutput::Disable()
{
    // Nothing to do.
}
//----------------------------------------------------------------------------