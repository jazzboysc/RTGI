//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "RendererInput.h"

using namespace RTGI;

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