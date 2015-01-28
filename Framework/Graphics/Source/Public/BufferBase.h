//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_BufferBase_H
#define RTGI_BufferBase_H

#include "RefObject.h"
#include "GPUResource.h"

namespace RTGI
{

enum BufferFormat
{
    BF_Unknown = -1,
    BF_R,
    BF_RG,
    BF_RGB,
    BF_RGBA,
    BF_RGBF,
    BF_RGBAF,
    BF_R32UI,
    BF_R32F,
    BF_Depth,
    BufferFormat_Max
};

enum BufferInternalFormat
{
    BIF_Unknown = -1,
    BIF_RGB8,
    BIF_RGBA8,
    BIF_RGB32F,
    BIF_RGBA32F,
    BIF_RGBA32UI,
    BIF_RGB16F,
    BIF_RGBA16F,
    BIF_R32UI,
    BIF_R32F,
    BIF_RG32F,
    BIF_Depth24,
    BufferInternalFormat_Max
};

enum BufferComponentType
{
    BCT_Unknown = -1,
    BCT_Unsigned_Byte,
    BCT_Unsigned_Int,
    BCT_Float,
    BufferComponentType_Max
};

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
    BF_Bind,
    BF_BindIndex,
    BF_BindToIndirect,
    BF_BindIndexToIndirect,
    BindingFlag_Max
};

enum BufferUsage
{
    BU_Static_Read = 0,
    BU_Static_Copy,
    BU_Static_Draw,
    BU_Dynamic_Read,
    BU_Dynamic_Copy,
    BU_Dynamic_Draw,
    BufferUsage_Max
};

enum BufferAccess
{
    BA_Read_Only = 0,
    BA_Write_Only,
    BA_Read_Write,
    BufferAccess_Max
};

/*
Filter Combination                     | Bilinear | Bilinear | Mipmapping
(MAG_FILTER / MIN_FILTER)              | (Near)   | (FAR)    |
---------------------------------------+----------+----------+------------
GL_NEAREST / GL_NEAREST_MIPMAP_NEAREST | Off      | Off      | Standard
GL_NEAREST / GL_LINEAR_MIPMAP_NEAREST  | Off      | On       | Standard
GL_NEAREST / GL_NEAREST_MIPMAP_LINEAR  | Off      | Off      | Trilinear filtering
GL_NEAREST / GL_LINEAR_MIPMAP_LINEAR   | Off      | On       | Trilinear filtering
GL_NEAREST / GL_NEAREST                | Off      | Off      | None
GL_NEAREST / GL_LINEAR                 | Off      | On       | None
GL_LINEAR / GL_NEAREST_MIPMAP_NEAREST  | On       | Off      | Standard
GL_LINEAR / GL_LINEAR_MIPMAP_NEAREST   | On       | On       | Standard
GL_LINEAR / GL_NEAREST_MIPMAP_LINEAR   | On       | Off      | Trilinear filtering
GL_LINEAR / GL_LINEAR_MIPMAP_LINEAR    | On       | On       | Trilinear filtering
GL_LINEAR / GL_NEAREST                 | On       | Off      | None
GL_LINEAR / GL_LINEAR                  | On       | On       | None
*/

enum FilterType
{
    FT_Nearest = 0,
    FT_Linear,
    FT_Nearest_Nearest,
    FT_Linear_Nearest,
    FT_Nearest_Linear,
    FT_Linear_Linear,
    FilterType_Max
};

enum WrapType
{
    WT_Clamp = 0,
    WT_Repeat,
    WrapType_Max
};

struct SamplerDesc
{
    FilterType MinFilter;
    FilterType MagFilter;
    FilterType MipMap;
    WrapType WrapS;
    WrapType WrapT;
    WrapType WrapR;
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