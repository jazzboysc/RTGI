//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_RendererOutput_H
#define RTGI_RendererOutput_H

#include "RefObject.h"
#include "IndexableBuffer.h"
#include <string>

namespace RTGI
{

enum RendererOutputType
{
    ROT_Buffer = 0,
    ROT_Texture
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class RendererOutput : public RefObject
{
public:
    RendererOutput(const std::string& name, BufferBase* outputBuffer, 
        RendererOutputType outputType = ROT_Texture, 
        BindingFlag flag = BF_Bindless, BufferType viewType = BT_Unknown,
        unsigned int binding = 0, bool reset = false, int resetValue = 0);
    virtual ~RendererOutput();

    void Enable();
    void Disable();

    std::string Name;
    BufferBasePtr OutputBuffer;
    BufferViewPtr OutputBufferView;
    RendererOutputType OutputType;
    BindingFlag Flag;
    unsigned int Binding;
    bool Reset;
    int ResetValue;

private:
    RendererOutput();
};

typedef RefPointer<RendererOutput> RendererOutputPtr;

}

#endif