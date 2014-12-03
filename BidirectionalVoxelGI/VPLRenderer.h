#ifndef RTGI_VPLRenderer_H
#define RTGI_VPLRenderer_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/03/2014
//----------------------------------------------------------------------------
class VPLRenderer : public SubRenderer
{
public:
    VPLRenderer(RenderSet* renderSet = 0);
    virtual ~VPLRenderer();

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