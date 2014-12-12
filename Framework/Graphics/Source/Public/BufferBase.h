//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_BufferBase_H
#define RTGI_BufferBase_H


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
    BF_Bindless       = 0,
    BF_Bind,
    BF_BindIndex,
    BF_BindToIndirect,
    BF_Max
};

enum BufferUsage
{
    BU_Static_Read     = 0,
    BU_Static_Copy,
    BU_Static_Draw,
    BU_Dynamic_Read,
    BU_Dynamic_Copy,
    BU_Dynamic_Draw,
    BU_Max
};

enum FilterType
{
    FT_Nearest = 0,
    FT_Linear,
    FT_Max
};

enum WrapType
{
    WT_Clamp =  0,
    WT_Repeat,
    WT_Max
};

struct SamplerDesc
{
    FilterType MinFilter;
    FilterType MagFilter;
    WrapType WrapS;
    WrapType WrapT;
};

struct RendererInputDataView
{
    RendererDataType Type;
    BindingFlag BindingType;
    int BindingSlot;
    SamplerDesc Sampler;
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
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