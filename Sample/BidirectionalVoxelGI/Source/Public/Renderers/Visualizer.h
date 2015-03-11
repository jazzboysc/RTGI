#ifndef RTGI_Visualizer_H
#define RTGI_Visualizer_H

#include "GraphicsFrameworkHeader.h"
#include "GridVoxelizer.h"
#include "SVOVoxelizer.h"
#include "ShadowMapRenderer.h"
#include "RSMRenderer.h"
#include "DirectLightingRenderer.h"
#include "IndirectLightingRenderer.h"

namespace RTGI
{

#define RTGI_Visualizer_IndirectCommandBuffer_Name "IndirectCommandBuffer"

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/06/2014
//----------------------------------------------------------------------------
class GatherVoxelBuffer : public ComputeTask
{
public:
    GatherVoxelBuffer();
    ~GatherVoxelBuffer();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnPreDispatch(unsigned int pass);
    virtual void OnPostDispatch(unsigned int pass);

    AABB* SceneBB;

private:
    ShaderUniform mSceneBBMinLoc;
    ShaderUniform mSceneBBExtensionLoc;
};

typedef RefPointer<GatherVoxelBuffer> GatherVoxelBufferPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/29/2014
//----------------------------------------------------------------------------
class VisualizerScreenQuad : public ScreenQuad
{
public:
    VisualizerScreenQuad(Material* material);
    virtual ~VisualizerScreenQuad();

    // Implement base class interface.
    virtual void OnUpdateShaderConstants(int technique, int pass);
    virtual void OnGetShaderConstants();

    Texture2DPtr TempTexture;
    Texture2DPtr TempTexture2;
    Texture2DPtr GBufferPositionTexture;
    Texture2DPtr GBufferNormalTexture;
    Texture2DArrayPtr TempTextureArray;
    int ShowMode;
    int TextureArrayIndex;
    AABB* SceneBB;
    StructuredBufferPtr VoxelBuffer;
    int VoxelGridDim;
    float PositionThreshold;
    float NormalThreshold;
    float MaxRadiance;
    int KernelSize;

private:
    ShaderUniform mShowModeLoc;
    ShaderUniform mTempSamplerLoc;
    ShaderUniform mTempSampler2Loc;
    ShaderUniform mTempSamplerArrayLoc;
    ShaderUniform mTextureArrayIndexLoc;
    ShaderUniform mSceneBBMinLoc;
    ShaderUniform mSceneBBExtensionLoc;
    ShaderUniform mDimLoc;
    ShaderUniform mPositionSamplerLoc;
    ShaderUniform mNormalSamplerLoc;
    ShaderUniform mPositionThresholdLoc;
    ShaderUniform mNormalThresholdLoc;
    ShaderUniform mMaxRadianceLoc;
    ShaderUniform mKernelSizeLoc;
};

typedef RefPointer<VisualizerScreenQuad> VisualizerScreenQuadPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/07/2014
//----------------------------------------------------------------------------
class VoxelCubeTriMesh : public TriangleMesh
{
public:
    VoxelCubeTriMesh(Material* material, Camera* camera);
    virtual ~VoxelCubeTriMesh();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnUpdateShaderConstants(int technique, int pass);

    vec3 MaterialColor;
};

typedef RefPointer<VoxelCubeTriMesh> VoxelCubeTriMeshPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 02/02/2015
//----------------------------------------------------------------------------
class SVOCubeMesh : public QuadMesh
{
public:
    SVOCubeMesh(Material* material, Camera* camera);
    virtual ~SVOCubeMesh();

    // Implement base class interface.
    virtual void OnGetShaderConstants();
    virtual void OnUpdateShaderConstants(int technique, int pass);

    AABB* SceneBB;

private:
    ShaderUniform mSceneBBMinLoc;
    ShaderUniform mVoxelExtensionLoc;
};

typedef RefPointer<SVOCubeMesh> SVOCubeMeshPtr;

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class Visualizer : public SubRenderer
{
public:
    enum ShowMode
    {
        SM_VoxelBuffer,
        SM_VoxelGrid,
        SM_SVOGrid,
        SM_Shadow,
        SM_VPLShadow,
        SM_GBufferPosition,
        SM_GBufferNormal,
        SM_GBufferAlbedo,
        SM_GBufferRPC,
        SM_RSMPosition,
        SM_RSMNormal,
        SM_RSMFlux,
        SM_DirectLighting,
        SM_IndirectLighting,
        SM_FilteredIndirectLighting,
        SM_Final
    };

    Visualizer(GPUDevice* device, RenderSet* renderSet = 0);
    virtual ~Visualizer();

    void Initialize(GPUDevice* device, Voxelizer* voxelizer, 
        ShadowMapRenderer* shadowMapRenderer, GBufferRenderer* gbufferRenderer,
        RSMRenderer* rsmRenderer, DirectLightingRenderer* directLightingRenderer, 
        IndirectLightingRenderer* indirectLightingRenderer, AABB* sceneBB, 
        int voxelGridDim, int voxelGridLocalGroupDim, Camera* mainCamera);

    void Render(int technique, int pass);

    // Implement base class interface.
    void OnRender(int technique, int pass, Camera* camera);

    void SetShowMode(ShowMode mode);

private:
    Voxelizer::VoxelizerType mVoxelizerType;
    int mVoxelGridDim;
    int mVoxelGridLocalGroupDim;
    int mGlobalDim;

    ShowMode mShowMode;

    VisualizerScreenQuadPtr mScreenQuad;

    Texture2DPtr mShadowMapTexture;
    Texture2DPtr mGBufferPositionTexture;
    Texture2DPtr mGBufferNormalTexture;
    Texture2DPtr mGBufferAlbedoTexture;
    Texture2DPtr mGBufferRPCTexture;
    Texture2DArrayPtr mRSMPositionTextureArray;
    Texture2DArrayPtr mRSMNormalTextureArray;
    Texture2DArrayPtr mRSMFluxTextureArray;
    Texture2DPtr mDirectLightingTexture;
    Texture2DPtr mIndirectLightingTexture;

    // Grid voxelizer visualization.
    StructuredBufferPtr mVoxelBuffer;
    VoxelCubeTriMeshPtr mVoxelCubeModel;
    GatherVoxelBufferPtr mGatherVoxelBufferTask;

    // SVO voxelizer visualization.
    StructuredBufferPtr mVoxelFragmentListBuffer;
    StructuredBufferPtr mSVOBuffer;
    UniformBufferPtr mSVOUniformBuffer;
    SVOCubeMeshPtr mSVONodeCubeModel;
};

typedef RefPointer<Visualizer> VisualizerPtr;

}

#endif