//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "RendererInput.h"

using namespace RTGI;

//----------------------------------------------------------------------------
void EnableTexture(RendererInput* input)
{

}
//----------------------------------------------------------------------------
void EnableBuffer(RendererInput* input)
{

}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
RendererInput::RendererInput(const std::string& name,
    BufferBase* inputBuffer, RendererDataView* view)
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
        assert(false);
        break;
    }
}
//----------------------------------------------------------------------------
void RendererInput::Disable()
{
    // Nothing to do.
}
//----------------------------------------------------------------------------