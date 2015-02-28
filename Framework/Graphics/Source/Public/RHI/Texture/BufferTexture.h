//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_BufferTexture_H
#define RTGI_BufferTexture_H

#include "Texture.h"
#include "TextureBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/27/2015
//----------------------------------------------------------------------------
class BufferTexture : public Texture
{
public:
    BufferTexture();
    ~BufferTexture();

    bool LoadFromTextureBuffer(GPUDevice* device, TextureBuffer* textureBuffer,
        BufferInternalFormat internalFormat);
};

typedef RefPointer<BufferTexture> BufferTexturePtr;

}

#endif