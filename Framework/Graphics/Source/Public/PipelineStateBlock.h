//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_PipelineStateBlock_H
#define RTGI_PipelineStateBlock_H


#include "RefObject.h"

namespace RTGI
{

enum PipelineBit
{
    PB_Rasterizer = 1,
    PB_OutputMerger = 2
};

enum RasterizerBit
{
    RB_Viewport = 1
};

enum OutputMergerBit
{
    OMB_Clear = 1
};

struct ViewportState
{
    GLint X;
    GLint Y;
    GLsizei Width;
    GLsizei Height;
};

struct RasterizerState
{
    unsigned int Flag;
    ViewportState Viewport;
};

struct OutputMergerState
{
    unsigned int Flag;
    GLbitfield ClearMask;
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
struct PipelineStateBlock : public RefObject
{
    PipelineStateBlock();

    unsigned int Flag;
    RasterizerState Rasterizer;
    OutputMergerState OutputMerger;
};

typedef RefPointer<PipelineStateBlock> PipelineStateBlockPtr;

}

#endif