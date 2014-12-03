//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Texture_H
#define RTGI_Texture_H

#include "FrameworkCommon.h"
#include "BufferBase.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/07/2013
//----------------------------------------------------------------------------
class Texture : public BufferBase
{
public:
    enum TextureFormat
    {
        TF_RGB,
        TF_RGBA,
        TF_RGBF,
        TF_RGBAF,
        TF_R32UI,
        TF_R32F,
        TF_Depth
    };

    enum TextureType
    {
        TT_Texture1D = 0,
        TT_Texture2D,
        TT_Texture3D,
        TT_TextureCube,
        TT_Texture2DArray,
        TT_Max
    };

	Texture();
	virtual ~Texture();

    bool IsRenderTarget;
    bool IsHDRTexture;
    bool IsTextureBuffer;

    TextureFormat RTFormat;

	GLuint GetTexture() const;
    virtual TextureType GetType() = 0;

protected:
	GLuint mTexture;
    TextureType mType;
};

typedef RefPointer<Texture> TexturePtr;

}

#endif