//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "RendererOutput.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RendererOutput::RendererOutput(const std::string& name, 
    BufferBase* outputBuffer, bool isTexture, BindingFlag flag, 
    unsigned int binding)
{
    Name = name;
    OutputBuffer = outputBuffer;
    IsTexture = isTexture;
    Flag = flag;
    Binding = binding;
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
}
//----------------------------------------------------------------------------
void RendererOutput::Disable()
{
    // Nothing to do.
}
//----------------------------------------------------------------------------