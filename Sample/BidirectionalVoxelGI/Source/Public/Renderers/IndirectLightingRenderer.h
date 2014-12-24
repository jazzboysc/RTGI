#ifndef RTGI_IndirectLightingRenderer_H
#define RTGI_IndirectLightingRenderer_H

#include "GraphicsFrameworkHeader.h"
#include "ScreenQuad.h"
#include "Texture2D.h"
#include "StructuredBuffer.h"
#include "VPLGenerator.h"

namespace RTGI
{

#define RTGI_IndirectLightingRenderer_IndirectLighting_Name "IndirectLighting"

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class IndirectLightingScreenQuad : public ScreenQuad
{
public:
    IndirectLightingScreenQuad(Material* material);
    virtual ~IndirectLightingScreenQuad();

    // Implement base class interface.
    virtual void OnUpdateShaderConstants(int technique, int pass);
    virtual void OnGetShaderConstants();

    int VPLCount;
    float BounceSingularity;

private:
    ShaderUniform mVPLCountLoc;
    ShaderUniform mBounceSingularityLoc;
    ShaderUniform mGBufferPositionSamplerLoc;
    ShaderUniform mGBufferNormalSamplerLoc;
    ShaderUniform mGBufferAlbedoSamplerLoc;
};

typedef RefPointer<IndirectLightingScreenQuad> IndirectLightingScreenQuadPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/03/2014
//----------------------------------------------------------------------------
class IndirectLightingRenderer : public SubRenderer
{
public:
    IndirectLightingRenderer(RenderSet* renderSet = 0);
    virtual ~IndirectLightingRenderer();

    void Initialize(GPUDevice* device, int width, int height, Texture::TextureFormat format,
        int vplCount);
    void SetInputs(GBufferRenderer* gbuffer, VPLGenerator* vplBuffer);
    void Render();

private:
    PipelineStateBlockPtr mPSB;
    IndirectLightingScreenQuadPtr mIndirectLightingScreenQuad;
};

typedef RefPointer<IndirectLightingRenderer> IndirectLightingRendererPtr;

}

#endif