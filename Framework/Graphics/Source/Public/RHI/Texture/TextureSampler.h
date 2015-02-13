//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_TextureSampler_H
#define RTGI_TextureSampler_H

#include "Texture.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 01/15/2015
//----------------------------------------------------------------------------
class TextureSampler
{
public:
    TextureSampler();
    ~TextureSampler();

    void SetTexture(Texture* texture);
    Texture* GetTexture() const;

    void Enable();

    SamplerDesc Sampler;

private:
    TexturePtr mTexture;
};

}

#endif