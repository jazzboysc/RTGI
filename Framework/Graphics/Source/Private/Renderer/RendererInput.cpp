//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "RendererInput.h"

using namespace RTGI;

//----------------------------------------------------------------------------
void EnableTexture(RendererInput* input)
{
    Texture* texture = (Texture*)(BufferBase*)input->InputBuffer;
    texture->BindToSampler(input->View.BindingSlot, &input->View.Sampler);
}
//----------------------------------------------------------------------------
void EnableBuffer(RendererInput* input)
{
    Buffer* buffer = (Buffer*)(BufferBase*)input->InputBuffer;
    switch( input->View.BindingType )
    {
    case BF_Bind:
        buffer->Bind();
        break;

    case BF_BindIndex:
        ((IndexableBuffer*)buffer)->Bind(input->View.BindingSlot);
        break;

    case BF_BindToIndirect:
        buffer->BindToIndirect();
        break;

    default:
        RTGI_ASSERT(false);
        break;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
RendererInput::RendererInput(const std::string& name,
    BufferBase* inputBuffer, RendererInputDataView* view)
{
    Name = name;
    InputBuffer = inputBuffer;
    View = *view;
}
//----------------------------------------------------------------------------
RendererInput::~RendererInput()
{
    InputBuffer = 0;
}
//----------------------------------------------------------------------------
void RendererInput::Enable()
{
    switch( View.Type )
    {
    case RDT_Texture:
        EnableTexture(this);
        break;

    case RDT_StructuredBuffer:
    case RDT_AtomicCounterBuffer:
    case RDT_UniformBuffer:
    case RDT_DispatchIndirectBuffer:
    case RDT_PixelBuffer:
    case RDT_TextureBuffer:
        EnableBuffer(this);
        break;

    default:
        RTGI_ASSERT(false);
        break;
    }
}
//----------------------------------------------------------------------------
void RendererInput::Disable()
{
    // Nothing to do.
}
//----------------------------------------------------------------------------