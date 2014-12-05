#ifndef RTGI_IndirectLightingRenderer_H
#define RTGI_IndirectLightingRenderer_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/03/2014
//----------------------------------------------------------------------------
class IndirectLightingRenderer : public SubRenderer
{
public:
    IndirectLightingRenderer(RenderSet* renderSet = 0);
    virtual ~IndirectLightingRenderer();

    void SetGBufferRenderer(GBufferRenderer* gbuffer);
    GBufferRenderer* GetGBufferRenderer() const;

    void CreateIndirectLightingBuffer(int width, int height, 
        Texture::TextureFormat format);
    void Render(int technique, int pass, Camera* camera);

private:
    GBufferRendererPtr mGBuffer;
};

}

#endif