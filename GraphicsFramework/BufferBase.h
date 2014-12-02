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
    RDT_Unknown                 = 0,
    RDT_Texture                 = 1,
    RDT_Image                   = 2,
    RDT_StructuredBuffer        = 4,
    RDT_AtomicCounterBuffer     = 8,
    RDT_UniformBuffer           = 16,
    RDT_DispatchIndirectBuffer  = 32,
    RDT_PixelBuffer             = 64,
    RDT_TextureBuffer           = 128
};

enum BindingFlag
{
    BF_Bindless = 0,
    BF_Bind = 1,
    BF_BindIndex = 2,
    BF_BindToIndirect = 3
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
    BindingFlag Binding;
    int BindingSlot;
    SamplerDesc SamplerFlag;
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