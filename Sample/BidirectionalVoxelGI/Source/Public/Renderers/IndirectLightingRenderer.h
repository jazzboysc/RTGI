#ifndef RTGI_IndirectLightingRenderer_H
#define RTGI_IndirectLightingRenderer_H

#include "GraphicsFrameworkHeader.h"
#include "ScreenQuad.h"
#include "Texture2D.h"
#include "StructuredBuffer.h"
#include "VPLGenerator.h"
#include "GridVoxelizer.h"

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
    int PatternSize;
    float BounceSingularity;
    AABB* SceneBB;
    int VoxelGridDim;
    bool VPLVisibilityTest;

private:
    ShaderUniform mVPLCountLoc;
    ShaderUniform mPatternSizeLoc;
    ShaderUniform mBounceSingularityLoc;
    ShaderUniform mGBufferPositionSamplerLoc;
    ShaderUniform mGBufferNormalSamplerLoc;
    ShaderUniform mGBufferAlbedoSamplerLoc;
    ShaderUniform mSceneBBCenterLoc;
    ShaderUniform mSceneBBExtensionLoc;
    ShaderUniform mDimLoc;
    ShaderUniform mVPLVisibilityTestLoc;
};

typedef RefPointer<IndirectLightingScreenQuad> IndirectLightingScreenQuadPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/03/2014
//----------------------------------------------------------------------------
class IndirectLightingRenderer : public SubRenderer
{
public:
    IndirectLightingRenderer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~IndirectLightingRenderer();

    void Initialize(GPUDevice* device, int width, int height, 
        BufferFormat format, int vplCount, int patternSize, 
        AABB* sceneBB, int voxelGridDim);
    void SetInputs(GBufferRenderer* gbuffer, VPLGenerator* vplBuffer, 
        GridVoxelizer* voxelBuffer);
    void Render();

    void VPLVisibilityTest(bool value);

private:
    PipelineStateBlockPtr mPSB;
    IndirectLightingScreenQuadPtr mIndirectLightingScreenQuad;
};

typedef RefPointer<IndirectLightingRenderer> IndirectLightingRendererPtr;

}

#endif