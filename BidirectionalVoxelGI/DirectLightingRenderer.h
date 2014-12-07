#ifndef RTGI_DirectLightingRenderer_H
#define RTGI_DirectLightingRenderer_H

#include "GraphicsFrameworkHeader.h"
#include "ShadowMapRenderer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class DirectLightingScreenQuad : public ScreenQuad
{
public:
    DirectLightingScreenQuad(Material* material);
    virtual ~DirectLightingScreenQuad();

    // Implement base class interface.
    virtual void OnUpdateShaderConstants(int technique, int pass);
    virtual void OnGetShaderConstants();

    Camera* LightProjector;
    vec3 LightColor;

private:
    GLint mGBufferPositionSamplerLoc;
    GLint mGBufferNormalSamplerLoc;
    GLint mGBufferAlbedoSamplerLoc;
    GLint mShadowMapSamplerLoc;
    GLint mLightProjectorViewLoc;
    GLint mLightPositionWorldLoc;
    GLint mLightColorLoc;
    GLint mLightProjectorNearFarLoc;
};

typedef RefPointer<DirectLightingScreenQuad> DirectLightingScreenQuadPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class DirectLightingRenderer : public SubRenderer
{
public:
    DirectLightingRenderer(RenderSet* renderSet = 0);
    virtual ~DirectLightingRenderer();

    void Initialize(int width, int height, Texture::TextureFormat format, 
        Camera* LightProjector);
    void SetInputs(GBufferRenderer* gbuffer, ShadowMapRenderer* shadowMap);
    void Render();

private:
    PipelineStateBlockPtr mPSB;
    DirectLightingScreenQuadPtr mDirectLightingScreenQuad;
};

typedef RefPointer<DirectLightingRenderer> DirectLightingRendererPtr;

}

#endif