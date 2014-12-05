//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "PipelineStateBlock.h"

using namespace RTGI;

//----------------------------------------------------------------------------
PipelineStateBlock::PipelineStateBlock()
{
    memset(this, 0, sizeof(PipelineStateBlock));
}
//----------------------------------------------------------------------------