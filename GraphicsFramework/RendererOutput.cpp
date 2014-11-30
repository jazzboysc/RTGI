//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "RendererOutput.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RendererOutput::RendererOutput(const std::string& name, 
    BufferBase* outputBuffer)
{
    Name = name;
    OutputBuffer = outputBuffer;
}
//----------------------------------------------------------------------------
RendererOutput::~RendererOutput()
{
    OutputBuffer = 0;
}
//----------------------------------------------------------------------------