//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Texture_H
#define RTGI_Texture_H


#include "BufferBase.h"
#include <GL/glew.h>

namespace RTGI
{

enum TextureFormat
{
    TF_Unknown = -1,
    TF_R,
    TF_RG,
    TF_RGB,
    TF_RGBA,
    TF_RGBF,
    TF_RGBAF,
    TF_R32UI,
    TF_R32F,
    TF_Depth,
    TF_Max
};

enum TextureInternalFormat
{
    TIF_Unknown = -1,
    TIF_RGB8,
    TIF_RGBA8,
    TIF_RGB32F,
    TIF_RGBA32F,
    TIF_RGB16F,
    TIF_RGBA16F,
    TIF_R32UI,
    TIF_R32F,
    TIF_RG32F,
    TIF_Depth24,
    TIF_Max
};

enum TextureComponentType
{
    TCT_Unknown = -1,
    TCT_Unsigned_Byte,
    TCT_Unsigned_Int,
    TCT_Float,
    TCT_Max
};

enum TextureType
{
    TT_Unknown = -1,
    TT_Texture1D,
    TT_Texture2D,
    TT_Texture3D,
    TT_TextureCube,
    TT_Texture2DArray,
    TT_Max
};

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/07/2013
//----------------------------------------------------------------------------
class Texture : public BufferBase
{
public:
	virtual ~Texture();

    bool IsRenderTarget;
    bool IsHDRTexture;
    bool IsTextureBuffer;

    TextureHandle* GetTextureHandle() const;
    TextureFormat GetFormat() const;
    TextureInternalFormat GetInternalFormat() const;
    TextureComponentType GetComponentType() const;
    TextureType GetType() const;

protected:
    Texture();

    TextureHandle* mTextureHandle;
    TextureFormat mFormat;
    TextureInternalFormat mInternalFormat;
    TextureComponentType mComponentType;
    TextureType mType;
};

typedef RefPointer<Texture> TexturePtr;

}

#endif