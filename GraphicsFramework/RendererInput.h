//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RendererInput_H
#define RTGI_RendererInput_H

#include "FrameworkCommon.h"
#include "RefObject.h"
#include "BufferBase.h"
#include <string>

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/30/2014
//----------------------------------------------------------------------------
class RendererInput : public RefObject
{
public:
    RendererInput(const std::string& name, BufferBase* inputBuffer, 
        RendererInputDataView* view);
    virtual ~RendererInput();

    void Enable();
    void Disable();

    std::string Name;
    BufferBasePtr InputBuffer;
    RendererInputDataView View;
};

typedef RefPointer<RendererInput> RendererInputPtr;

}

#endif