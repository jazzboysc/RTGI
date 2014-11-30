//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_BufferBase_H
#define RTGI_BufferBase_H

#include "FrameworkCommon.h"
#include "RefObject.h"

namespace RTGI
{

enum RendererDataType
{
    RIT_Unknown             = 0,
    RIT_Texture             = 1,
    RIT_Image               = 2,
    RIT_StructuredBuffer    = 4,
    RIT_AtomicCounterBuffer = 8,
    RIT_ConstantBuffer      = 16,
    RIT_CommandBuffer       = 32
};

enum BufferUsage
{
    BU_Static_Read     = 0,
    BU_Static_Copy     = 1,
    BU_Dynamic_Read    = 2,
    BU_Dynamic_Copy    = 3,
    BU_MAX             = 4
};

struct SamplerDesc
{

};

struct RendererDataView
{
    RendererDataType Type;
    int BindingSlot;
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2013
//----------------------------------------------------------------------------
class BufferBase : public RefObject
{
public:
    BufferBase();
    virtual ~BufferBase();


};

typedef RefPointer<BufferBase> BufferBasePtr;

}

#endif