//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "PipelineStateBlock.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ViewportState::ViewportState()
{
    memset(this, 0, sizeof(ViewportState));
}
//----------------------------------------------------------------------------
RasterizerState::RasterizerState()
{
    Flag = 0;
}
//----------------------------------------------------------------------------
OutputMergerState::OutputMergerState()
{
    Flag = 0;
    ClearMask = 0;
}
//----------------------------------------------------------------------------
PipelineStateBlock::PipelineStateBlock()
{
    Flag = 0;
}
//----------------------------------------------------------------------------