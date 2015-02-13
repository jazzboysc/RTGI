//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "TextureSampler.h"

using namespace RTGI;

//----------------------------------------------------------------------------
TextureSampler::TextureSampler()
{
}
//----------------------------------------------------------------------------
TextureSampler::~TextureSampler()
{
    mTexture = 0;
}
//----------------------------------------------------------------------------
void TextureSampler::SetTexture(Texture* texture)
{
    mTexture = texture;
}
//----------------------------------------------------------------------------
Texture* TextureSampler::GetTexture() const
{
    return mTexture;
}
//----------------------------------------------------------------------------
void TextureSampler::Enable()
{
}
//----------------------------------------------------------------------------