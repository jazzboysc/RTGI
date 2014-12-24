#ifndef RTGI_DirectLightingRenderer_H
#define RTGI_DirectLightingRenderer_H

#include "GraphicsFrameworkHeader.h"
#include "ShadowMapRenderer.h"

namespace RTGI
{

#define RTGI_DirectLightingRenderer_DirectLighting_Name "DirectLighting"

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
    ShaderUniform mGBufferPositionSamplerLoc;
    ShaderUniform mGBufferNormalSamplerLoc;
    ShaderUniform mGBufferAlbedoSamplerLoc;
    ShaderUniform mShadowMapSamplerLoc;
    ShaderUniform mLightProjectorViewLoc;
    ShaderUniform mLightPositionWorldLoc;
    ShaderUniform mLightColorLoc;
    ShaderUniform mLightProjectorNearFarLoc;
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

    void Initialize(GPUDevice* device, int width, int height, Texture::TextureFormat format, 
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