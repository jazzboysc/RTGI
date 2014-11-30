//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RendererOutput_H
#define RTGI_RendererOutput_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "BufferBase.h"
#include <string>

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2013
//----------------------------------------------------------------------------
class RendererOutput : public RefObject
{
public:
    RendererOutput(const std::string& name, BufferBase* outputBuffer);
    virtual ~RendererOutput();

    std::string Name;
    BufferBasePtr OutputBuffer;
};

typedef RefPointer<RendererOutput> RendererOutputPtr;

}

#endif