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

enum TextureType
{
    TT_Unknown = -1,
    TT_Texture1D,
    TT_Texture2D,
    TT_Texture3D,
    TT_TextureCube,
    TT_Texture2DArray,
    TT_BufferTexture,
    TextureType_Max
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
    bool HasMipMap;

    inline TextureHandle* GetTextureHandle() const { return mTextureHandle; }
    inline BufferFormat GetFormat() const { return mFormat; }
    inline BufferInternalFormat GetInternalFormat() const { return mInternalFormat; }
    inline BufferComponentType GetComponentType() const { return mComponentType; }
    inline TextureType GetType() const { return mType; }

	void BindToImageUnit(unsigned int unit, BufferAccess access, bool layered = false);
    void BindToSampler(unsigned int index, SamplerDesc* sampler);

    void GenerateMipmap();

protected:
    Texture();

    TextureHandle* mTextureHandle;
    BufferFormat mFormat;
    BufferInternalFormat mInternalFormat;
    BufferComponentType mComponentType;
    TextureType mType;
};

typedef RefPointer<Texture> TexturePtr;

}

#endif