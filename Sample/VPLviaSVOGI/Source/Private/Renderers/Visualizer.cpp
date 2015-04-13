#include "Visualizer.h"
#include "VPLviaSVOGIApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GatherVoxelBuffer::GatherVoxelBuffer()
{
}
//----------------------------------------------------------------------------
GatherVoxelBuffer::~GatherVoxelBuffer()
{
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnGetShaderConstants()
{
    ComputePass* p = (ComputePass*)GetPass(0);
    ShaderProgram* program = p->GetShaderProgram();

    program->GetUniformLocation(&mSceneBBMinLoc, "SceneBBMin");
    program->GetUniformLocation(&mSceneBBExtensionLoc, "SceneBBExtension");
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnPreDispatch(unsigned int pass)
{
    vec3 min = SceneBB->Min;
    vec3 extension = SceneBB->GetExtension();
    mSceneBBMinLoc.SetValue(min);
    mSceneBBExtensionLoc.SetValue(extension);
}
//----------------------------------------------------------------------------
void GatherVoxelBuffer::OnPostDispatch(unsigned int pass)
{
    glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
VisualizerScreenQuad::VisualizerScreenQuad(Material* material)
    :
    ScreenQuad(material, 0)
{
    ShowMode = 0;
    TextureArrayIndex = 0;
    SceneBB = 0;
}
//----------------------------------------------------------------------------
VisualizerScreenQuad::~VisualizerScreenQuad()
{
    TempTexture = 0;
    TempTexture2 = 0;
    GBufferPositionTexture = 0;
    GBufferNormalTexture = 0;
    TempTextureArray = 0;
    VoxelBuffer = 0;
}
//----------------------------------------------------------------------------
void VisualizerScreenQuad::OnUpdateShaderConstants(int technique, int pass)
{
    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    if( pass == 0 )
    {
        mTempSamplerSM0Loc.SetValue(0);
        if( TempTexture )
        {
            TempTexture->BindToSampler(0, &sampler);
        }
    }
    else if( pass == 1 )
    {
        mTempSamplerArraySM1Loc.SetValue(0);
        if( TempTextureArray )
        {
            TempTextureArray->BindToSampler(0, &sampler);
        }
        mTextureArrayIndexSM1Loc.SetValue(TextureArrayIndex);
    }
    else if( pass == 2 )
    {
        mTempSamplerSM2Loc.SetValue(0);
        if( TempTexture )
        {
            TempTexture->BindToSampler(0, &sampler);
        }

        mTempSampler2SM2Loc.SetValue(1);
        if( TempTexture2 )
        {
            TempTexture2->BindToSampler(1, &sampler);
        }

        mPositionSamplerSM2Loc.SetValue(2);
        if( GBufferPositionTexture )
        {
            GBufferPositionTexture->BindToSampler(2, &sampler);
        }

        mNormalSamplerSM2Loc.SetValue(3);
        if( GBufferNormalTexture )
        {
            GBufferNormalTexture->BindToSampler(3, &sampler);
        }

        mPositionThresholdSM2Loc.SetValue(PositionThreshold);
        mNormalThresholdSM2Loc.SetValue(NormalThreshold);
        mKernelSizeSM2Loc.SetValue(KernelSize);
    }
    else if( pass == 3 )
    {
        mTempSamplerSM3Loc.SetValue(0);
        if( TempTexture )
        {
            TempTexture->BindToSampler(0, &sampler);
        }

        if( SceneBB )
        {
            mSceneBBMinSM3Loc.SetValue(SceneBB->Min);
            mSceneBBExtensionSM3Loc.SetValue(SceneBB->GetExtension());
        }
    }
    else if( pass == 4 )
    {
        mTempSamplerSM4Loc.SetValue(0);
        if( TempTexture )
        {
            TempTexture->BindToSampler(0, &sampler);
        }

        if( SceneBB )
        {
            mSceneBBMinSM4Loc.SetValue(SceneBB->Min);
            mSceneBBExtensionSM4Loc.SetValue(SceneBB->GetExtension());
        }

        mDimSM4Loc.SetValue(VoxelGridDim);
    }
    else if( pass == 5 )
    {
        mTempSamplerSM5Loc.SetValue(0);
        if( TempTexture )
        {
            TempTexture->BindToSampler(0, &sampler);
        }

        mPositionSamplerSM5Loc.SetValue(1);
        if( GBufferPositionTexture )
        {
            GBufferPositionTexture->BindToSampler(1, &sampler);
        }

        mNormalSamplerSM5Loc.SetValue(2);
        if( GBufferNormalTexture )
        {
            GBufferNormalTexture->BindToSampler(2, &sampler);
        }

        mPositionThresholdSM5Loc.SetValue(PositionThreshold);
        mNormalThresholdSM5Loc.SetValue(NormalThreshold);
        mKernelSizeSM5Loc.SetValue(KernelSize);
    }

    if( VoxelBuffer )
    {
        VoxelBuffer->Bind(0);
    }
}
//----------------------------------------------------------------------------
void VisualizerScreenQuad::OnGetShaderConstants()
{
    // SM0
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerSM0Loc, "tempSampler");

    // SM1
    program = mMaterial->GetProgram(0, 1);
    program->GetUniformLocation(&mTempSamplerArraySM1Loc, "tempSamplerArray");
    program->GetUniformLocation(&mTextureArrayIndexSM1Loc, "TextureArrayIndex");

    // SM2
    program = mMaterial->GetProgram(0, 2);
    program->GetUniformLocation(&mTempSamplerSM2Loc, "tempSampler");
    program->GetUniformLocation(&mTempSampler2SM2Loc, "tempSampler2");
    program->GetUniformLocation(&mPositionSamplerSM2Loc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerSM2Loc, "normalSampler");
    program->GetUniformLocation(&mPositionThresholdSM2Loc, "positionThreshold");
    program->GetUniformLocation(&mNormalThresholdSM2Loc, "normalThreshold");
    program->GetUniformLocation(&mKernelSizeSM2Loc, "kernelSize");

    // SM3
    program = mMaterial->GetProgram(0, 3);
    program->GetUniformLocation(&mTempSamplerSM3Loc, "tempSampler");
    program->GetUniformLocation(&mSceneBBMinSM3Loc, "SceneBBMin");
    program->GetUniformLocation(&mSceneBBExtensionSM3Loc, "SceneBBExtension");

    // SM4
    program = mMaterial->GetProgram(0, 4);
    program->GetUniformLocation(&mTempSamplerSM4Loc, "tempSampler");
    program->GetUniformLocation(&mSceneBBMinSM4Loc, "SceneBBMin");
    program->GetUniformLocation(&mSceneBBExtensionSM4Loc, "SceneBBExtension");
    program->GetUniformLocation(&mDimSM4Loc, "dim");

    // SM5
    program = mMaterial->GetProgram(0, 5);
    program->GetUniformLocation(&mTempSamplerSM5Loc, "tempSampler");
    program->GetUniformLocation(&mPositionSamplerSM5Loc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerSM5Loc, "normalSampler");
    program->GetUniformLocation(&mPositionThresholdSM5Loc, "positionThreshold");
    program->GetUniformLocation(&mNormalThresholdSM5Loc, "normalThreshold");
    program->GetUniformLocation(&mKernelSizeSM5Loc, "kernelSize");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
VoxelCubeTriMesh::VoxelCubeTriMesh(Material* material, Camera* camera)
    :
    TriangleMesh(material, camera),
    MaterialColor(0.75f, 0.75f, 0.75f)
{
}
//----------------------------------------------------------------------------
VoxelCubeTriMesh::~VoxelCubeTriMesh()
{
}
//----------------------------------------------------------------------------
void VoxelCubeTriMesh::OnGetShaderConstants()
{
    TriangleMesh::OnGetShaderConstants();
}
//----------------------------------------------------------------------------
void VoxelCubeTriMesh::OnUpdateShaderConstants(int technique, int pass)
{
    TriangleMesh::OnUpdateShaderConstants(technique, pass);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
SVOCubeMesh::SVOCubeMesh(Material* material, Camera* camera)
    :
    QuadMesh(material, camera)
{
    SceneBB = 0;
}
//----------------------------------------------------------------------------
SVOCubeMesh::~SVOCubeMesh()
{
}
//----------------------------------------------------------------------------
void SVOCubeMesh::OnGetShaderConstants()
{
    QuadMesh::OnGetShaderConstants();

    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mSceneBBMinLoc, "SceneBBMin");
    program->GetUniformLocation(&mVoxelExtensionLoc, "VoxelExtension");
}
//----------------------------------------------------------------------------
void SVOCubeMesh::OnUpdateShaderConstants(int technique, int pass)
{
    QuadMesh::OnUpdateShaderConstants(technique, pass);

    vec3 sceneBBMin = SceneBB->Min;
    vec3 voxelExtension = (SceneBB->Max - SceneBB->Min);
    voxelExtension /= VPLviaSVOGI::VOXEL_DIMENSION;

    mSceneBBMinLoc.SetValue(sceneBBMin);
    mVoxelExtensionLoc.SetValue(voxelExtension);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
VPLPointSet::VPLPointSet(Material* material, Camera* camera)
    :
    PointSet(material)
{
    SetCamera(camera);
    CurVPLSubsetID = 0;
    VPLCount = 0;
    PatternSize = 0;
    ShowVPLSubset = false;
    ShowVPLFluxContrast = false;
}
//----------------------------------------------------------------------------
VPLPointSet::~VPLPointSet()
{

}
//----------------------------------------------------------------------------
void VPLPointSet::OnGetShaderConstants()
{
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mViewLoc, "View");
    program->GetUniformLocation(&mProjLoc, "Proj");
    program->GetUniformLocation(&mCurVPLSubsetIDLoc, "CurVPLSubsetID");
    program->GetUniformLocation(&mVPLCountLoc, "VPLCount");
    program->GetUniformLocation(&mPatternSizeLoc, "PatternSize");
    program->GetUniformLocation(&mShowVPLSubsetLoc, "ShowVPLSubset");
    program->GetUniformLocation(&mShowVPLSubsetLoc, "ShowVPLSubset");
    program->GetUniformLocation(&mShowVPLFluxContrastLoc, 
        "ShowVPLFluxContrast");
}
//----------------------------------------------------------------------------
void VPLPointSet::OnUpdateShaderConstants(int technique, int pass)
{
    if( mCamera )
    {
        mat4 viewTrans = mCamera->GetViewTransform();
        mViewLoc.SetValue(viewTrans);

        mat4 projTrans = mCamera->GetProjectionTransform();
        mProjLoc.SetValue(projTrans);
    }
    mCurVPLSubsetIDLoc.SetValue(CurVPLSubsetID);
    mVPLCountLoc.SetValue(VPLCount);
    mPatternSizeLoc.SetValue(PatternSize);
    mShowVPLSubsetLoc.SetValue(ShowVPLSubset);
    mShowVPLFluxContrastLoc.SetValue(ShowVPLFluxContrast);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
Visualizer::Visualizer(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
{
    mShowVPL = false;
    mShowVPLSubset = false;
    mVPLSubsetCount = 0;
    mCurVPLSubsetIndex = 0;
}
//----------------------------------------------------------------------------
Visualizer::~Visualizer()
{
    mVoxelBuffer = 0;

    mVPLBuffer = 0;
    mVPLPointSet = 0;

    mVoxelFragmentListBuffer = 0;
    mSVOBuffer = 0;
    mSVOUniformBuffer = 0;
    mSVONodeCubeModel = 0;

    mShadowMapTexture = 0;
    mGBufferPositionTexture = 0;
    mGBufferNormalTexture = 0;
    mGBufferAlbedoTexture = 0;
    mGBufferRPCTexture = 0;
    mRSMPositionTextureArray = 0;
    mRSMNormalTextureArray = 0;
    mRSMFluxTextureArray = 0;
    mDirectLightingTexture = 0;
    mIndirectLightingTexture = 0;

    mScreenQuad = 0;
    mVoxelCubeModel = 0;
}
//----------------------------------------------------------------------------
void Visualizer::Initialize(GPUDevice* device, Voxelizer* voxelizer, 
    VPLGenerator* vplGenerator, ShadowMapsGenerator* shadowMapsGenerator, 
    GBufferRenderer* gbufferRenderer, RSMRenderer* rsmRenderer, 
    DirectLightingRenderer* directLightingRenderer,
    IndirectLightingRenderer* indirectLightingRenderer, AABB* sceneBB, 
    int voxelGridDim, int voxelGridLocalGroupDim, Camera* mainCamera, 
    int patternSize, int vplCount)
{
    mVoxelizerType = voxelizer->GetVoxelizerType();
    mVoxelGridDim = voxelGridDim;
    mVoxelGridLocalGroupDim = voxelGridLocalGroupDim;
    mGlobalDim = mVoxelGridDim / mVoxelGridLocalGroupDim;

    ShaderProgramInfo tempResultSM0ProgramInfo;
    tempResultSM0ProgramInfo.VShaderFileName = "VPLviaSVOGI/vTempResult.glsl";
    tempResultSM0ProgramInfo.FShaderFileName = "VPLviaSVOGI/fTempResultSM0.glsl";
    tempResultSM0ProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                               ShaderType::ST_Fragment;
    Pass* passTempResultSM0 = new Pass(tempResultSM0ProgramInfo);

    ShaderProgramInfo tempResultSM1ProgramInfo;
    tempResultSM1ProgramInfo.VShaderFileName = "VPLviaSVOGI/vTempResult.glsl";
    tempResultSM1ProgramInfo.FShaderFileName = "VPLviaSVOGI/fTempResultSM1.glsl";
    tempResultSM1ProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                               ShaderType::ST_Fragment;
    Pass* passTempResultSM1 = new Pass(tempResultSM1ProgramInfo);

    ShaderProgramInfo tempResultSM2ProgramInfo;
    tempResultSM2ProgramInfo.VShaderFileName = "VPLviaSVOGI/vTempResult.glsl";
    tempResultSM2ProgramInfo.FShaderFileName = "VPLviaSVOGI/fTempResultSM2.glsl";
    tempResultSM2ProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                               ShaderType::ST_Fragment;
    Pass* passTempResultSM2 = new Pass(tempResultSM2ProgramInfo);

    ShaderProgramInfo tempResultSM3ProgramInfo;
    tempResultSM3ProgramInfo.VShaderFileName = "VPLviaSVOGI/vTempResult.glsl";
    tempResultSM3ProgramInfo.FShaderFileName = "VPLviaSVOGI/fTempResultSM3.glsl";
    tempResultSM3ProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                               ShaderType::ST_Fragment;
    Pass* passTempResultSM3 = new Pass(tempResultSM3ProgramInfo);

    ShaderProgramInfo tempResultSM4ProgramInfo;
    tempResultSM4ProgramInfo.VShaderFileName = "VPLviaSVOGI/vTempResult.glsl";
    tempResultSM4ProgramInfo.FShaderFileName = "VPLviaSVOGI/fTempResultSM4.glsl";
    tempResultSM4ProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                               ShaderType::ST_Fragment;
    Pass* passTempResultSM4 = new Pass(tempResultSM4ProgramInfo);

    ShaderProgramInfo tempResultSM5ProgramInfo;
    tempResultSM5ProgramInfo.VShaderFileName = "VPLviaSVOGI/vTempResult.glsl";
    tempResultSM5ProgramInfo.FShaderFileName = "VPLviaSVOGI/fTempResultSM5.glsl";
    tempResultSM5ProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                               ShaderType::ST_Fragment;
    Pass* passTempResultSM5 = new Pass(tempResultSM5ProgramInfo);

    Technique* techScreenQuad = new Technique();
    techScreenQuad->AddPass(passTempResultSM0);
    techScreenQuad->AddPass(passTempResultSM1);
    techScreenQuad->AddPass(passTempResultSM2);
    techScreenQuad->AddPass(passTempResultSM3);
    techScreenQuad->AddPass(passTempResultSM4);
    techScreenQuad->AddPass(passTempResultSM5);
    MaterialTemplate* mtScreenQuad = new MaterialTemplate();
    mtScreenQuad->AddTechnique(techScreenQuad);

    // Cache temp buffer and textures needed for visualization.
    mShadowMapTexture = 
        (Texture2D*)shadowMapsGenerator->GetFrameBufferTextureByName(
        RTGI_ShadowMapRenderer_ShadowMap_Name);
    mGBufferPositionTexture = 
        (Texture2D*)gbufferRenderer->GetFrameBufferTextureByName(
        RTGI_GBuffer_Position_Name);
    mGBufferNormalTexture = 
        (Texture2D*)gbufferRenderer->GetFrameBufferTextureByName(
        RTGI_GBuffer_Normal_Name);
    mGBufferAlbedoTexture = 
        (Texture2D*)gbufferRenderer->GetFrameBufferTextureByName(
        RTGI_GBuffer_Albedo_Name);
    mGBufferRPCTexture =
        (Texture2D*)gbufferRenderer->GetFrameBufferTextureByName(
        RTGI_GBuffer_RPC_Name);
    mRSMPositionTextureArray = 
        (Texture2DArray*)rsmRenderer->GetFrameBufferTextureByName(
        RTGI_RSMRenderer_RSMPosition_Name);
    mRSMNormalTextureArray = 
        (Texture2DArray*)rsmRenderer->GetFrameBufferTextureByName(
        RTGI_RSMRenderer_RSMNormal_Name);
    mRSMFluxTextureArray = 
        (Texture2DArray*)rsmRenderer->GetFrameBufferTextureByName(
        RTGI_RSMRenderer_RSMFlux_Name);
    mDirectLightingTexture = 
        (Texture2D*)directLightingRenderer->GetFrameBufferTextureByName(
        RTGI_DirectLightingRenderer_DirectLighting_Name);
    mIndirectLightingTexture = indirectLightingRenderer->IndirectLightingTexture;

    mVPLBuffer = (StructuredBuffer*)vplGenerator->GetGenericBufferByName(
        RTGI_VPLGenerator_VPLBuffer_Name);

    // Create VPL point set for VPL visualization.

    mVPLSubsetCount = patternSize*patternSize;

    ShaderProgramInfo showVPLProgramInfo;
    showVPLProgramInfo.VShaderFileName = "VPLviaSVOGI/vShowVPL.glsl";
    showVPLProgramInfo.FShaderFileName = "VPLviaSVOGI/fShowVPL.glsl";
    showVPLProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                         ShaderType::ST_Fragment;
    Pass* passShowVPL = new Pass(showVPLProgramInfo);

    Technique* techShowVPL = new Technique();
    techShowVPL->AddPass(passShowVPL);
    MaterialTemplate* mtShowVPL = new MaterialTemplate();
    mtShowVPL->AddTechnique(techShowVPL);

    float* vplPointSetVB = new float[4];
    memset(vplPointSetVB, 0, sizeof(float) * 4);
    Material* showVPLMaterial = new Material(mtShowVPL);
    mVPLPointSet = new VPLPointSet(showVPLMaterial, mainCamera);
    mVPLPointSet->LoadFromSystemMemory(1, vplPointSetVB, 4);
    mVPLPointSet->CreateDeviceResource(mDevice);
    mVPLPointSet->PointSize = 5.0f;
    mVPLPointSet->VPLCount = vplCount;
    mVPLPointSet->CurVPLSubsetID = mCurVPLSubsetIndex;
    mVPLPointSet->PatternSize = patternSize;
    SetShowVPLSubset(mShowVPLSubset);
    delete[] vplPointSetVB;

    if( mVoxelizerType == Voxelizer::VT_Grid )
    {
        ShaderProgramInfo showVoxelGridProgramInfo;
        showVoxelGridProgramInfo.VShaderFileName = 
            "VPLviaSVOGI/vShowVoxelGrid.glsl";
        showVoxelGridProgramInfo.FShaderFileName = 
            "VPLviaSVOGI/fShowVoxelGrid.glsl";
        showVoxelGridProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                                   ShaderType::ST_Fragment;
        Pass* passShowVoxelGrid = new Pass(showVoxelGridProgramInfo);

        Technique* techShowVoxelGrid = new Technique();
        techShowVoxelGrid->AddPass(passShowVoxelGrid);
        MaterialTemplate* mtShowVoxelGrid = new MaterialTemplate();
        mtShowVoxelGrid->AddTechnique(techShowVoxelGrid);

        // Cache grid voxelizer's voxel buffer.
        mVoxelBuffer = (StructuredBuffer*)voxelizer->GetGenericBufferByName(
            RTGI_GridVoxelizer_VoxelBuffer_Name);

        // Create gather voxel buffer task.
        ShaderProgramInfo gatherVoxelBufferProgramInfo;
        gatherVoxelBufferProgramInfo.CShaderFileName =
            "VPLviaSVOGI/cGatherVoxelBuffer.glsl";
        gatherVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;
        ComputePass* passGatherVoxelBuffer =
            new ComputePass(gatherVoxelBufferProgramInfo);
        mGatherVoxelBufferTask = new GatherVoxelBuffer();
        mGatherVoxelBufferTask->AddPass(passGatherVoxelBuffer);
        mGatherVoxelBufferTask->CreateDeviceResource(device);
        mGatherVoxelBufferTask->SceneBB = sceneBB;

        // Create indirect command buffer.
        int voxelCount = voxelGridDim * voxelGridDim * voxelGridDim;
        int bufferSize = sizeof(unsigned int) * 5 + sizeof(float) * 35
            + voxelCount*sizeof(float) * 4;
        AddGenericBufferTarget(RTGI_Visualizer_IndirectCommandBuffer_Name,
            RDT_StructuredBuffer, bufferSize, BU_Dynamic_Copy,
            BF_BindIndexTo, 1);

        // Create gathered voxel GPU memory allocator counter.
        bufferSize = sizeof(unsigned int);
        AddGenericBufferTarget("GPUMemoryAllocatorCounter",
            RDT_AtomicCounterBuffer, bufferSize, BU_Dynamic_Copy, BF_BindIndex,
            0, true, 0);

        // Create voxel cube model.
        Material* material = new Material(mtShowVoxelGrid);
        mVoxelCubeModel = new VoxelCubeTriMesh(material, mainCamera);
        mVoxelCubeModel->LoadFromPLYFile("box.ply");
        mVoxelCubeModel->GenerateNormals();
        mVoxelCubeModel->SetIndirectCommandBuffer(
            (StructuredBuffer*)GetGenericBufferByName(
            RTGI_Visualizer_IndirectCommandBuffer_Name), 0);
        mVoxelCubeModel->CreateDeviceResource(device);
    }
    else if( mVoxelizerType == Voxelizer::VT_SVO )
    {
        // Cache SVO buffers.
        mVoxelFragmentListBuffer = ((SVOVoxelizer*)voxelizer)->GetVoxelFragmentListBuffer();
        mSVOBuffer = ((SVOVoxelizer*)voxelizer)->GetSVOBuffer();
        mSVOUniformBuffer = ((SVOVoxelizer*)voxelizer)->GetSVOUniformBuffer();

        ShaderProgramInfo showSVOProgramInfo;
        showSVOProgramInfo.VShaderFileName = "VPLviaSVOGI/vShowSVO.glsl";
        showSVOProgramInfo.FShaderFileName = "VPLviaSVOGI/fShowSVO.glsl";
        showSVOProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                             ShaderType::ST_Fragment;
        Pass* passShowSVO = new Pass(showSVOProgramInfo);

        Technique* techShowSVO = new Technique();
        techShowSVO->AddPass(passShowSVO);
        MaterialTemplate* mtShowSVO = new MaterialTemplate();
        mtShowSVO->AddTechnique(techShowSVO);

        // Create SVO node cube model.
        std::vector<vec3> svoCubeVertices;
        svoCubeVertices.reserve(8);
        svoCubeVertices.push_back(vec3(-1.0f, 1.0f, 1.0f));
        svoCubeVertices.push_back(vec3(1.0f, 1.0f, 1.0f));
        svoCubeVertices.push_back(vec3(1.0f, 1.0f, -1.0f));
        svoCubeVertices.push_back(vec3(-1.0f, 1.0f, -1.0f));
        svoCubeVertices.push_back(vec3(-1.0f, -1.0f, 1.0f));
        svoCubeVertices.push_back(vec3(1.0f, -1.0f, 1.0f));
        svoCubeVertices.push_back(vec3(1.0f, -1.0f, -1.0f));
        svoCubeVertices.push_back(vec3(-1.0f, -1.0f, -1.0f));
        std::vector<unsigned int> svoCubeIndices;
        svoCubeIndices.reserve(24);
        static unsigned int indices[] = { 0, 1, 2, 3,
            0, 4, 5, 1,
            0, 3, 7, 4,
            2, 6, 7, 3,
            1, 5, 6, 2,
            4, 7, 6, 5 };
        for( int i = 0; i < 24; ++i )
        {
            svoCubeIndices.push_back(indices[i]);
        }

        Material* material = new Material(mtShowSVO);
        mSVONodeCubeModel = new SVOCubeMesh(material, mainCamera);
        mSVONodeCubeModel->LoadFromSystemMemory(svoCubeVertices, svoCubeIndices);
        mSVONodeCubeModel->SetIndirectCommandBuffer(mSVOBuffer, 16);
        mSVONodeCubeModel->CreateDeviceResource(device);
        mSVONodeCubeModel->SceneBB = sceneBB;
    }
    else
    {
        assert(false);
    }

    // Create screen quad.
    Material* material = new Material(mtScreenQuad);
    mScreenQuad = new VisualizerScreenQuad(material);
    mScreenQuad->LoadFromPLYFile("screenquad.ply");
    mScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mScreenQuad->CreateDeviceResource(device);
    mScreenQuad->SceneBB = sceneBB;
    mScreenQuad->VoxelBuffer = mVoxelBuffer;
    mScreenQuad->VoxelGridDim = voxelGridDim;
    mScreenQuad->GBufferPositionTexture = mGBufferPositionTexture;
    mScreenQuad->GBufferNormalTexture = mGBufferNormalTexture;
    mScreenQuad->PositionThreshold = 5.5f;
    mScreenQuad->NormalThreshold = 0.3f;
    mScreenQuad->KernelSize = patternSize;

    SetShowMode(SM_Final);
}
//----------------------------------------------------------------------------
void Visualizer::Render(int technique, int pass)
{
    SubRenderer::Render(technique, pass, SRO_GenericBuffer | SRO_BackBuffer, 
        0, 0);
}
//----------------------------------------------------------------------------
void Visualizer::OnRender(int, int, Camera*)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if( mVoxelizerType == Voxelizer::VT_Grid && mShowMode == SM_VoxelGrid )
    {
        mVoxelBuffer->Bind(0);

        // Gather voxel buffer pass.
        mGatherVoxelBufferTask->DispatchCompute(0, mGlobalDim, mGlobalDim, 
            mGlobalDim);

        // Show voxel grid.
        mVoxelCubeModel->Render(0, 0);
    }
    else if( mVoxelizerType == Voxelizer::VT_SVO && mShowMode == SM_SVOGrid )
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        mSVOUniformBuffer->Bind(0);
        mVoxelFragmentListBuffer->Bind(1);
        mSVOBuffer->Bind(3);
        mSVONodeCubeModel->Render(0, 0);
    }
    else
    {
        mScreenQuad->Render(0, mScreenQuad->ShowMode);
    }

    if( mShowVPL )
    {
        glDisable(GL_DEPTH_TEST);

        mVPLBuffer->Bind(0);
        mVPLPointSet->CurVPLSubsetID = mCurVPLSubsetIndex;
        mVPLPointSet->Render(0, 0);
    }
}
//----------------------------------------------------------------------------
void Visualizer::SetShowMode(ShowMode mode)
{
    mShowMode = mode;
    switch( mShowMode )
    {
    case SM_VoxelBuffer:
        mScreenQuad->ShowMode = 4;
        mScreenQuad->TempTexture = mGBufferPositionTexture;
        break;

    case SM_VoxelGrid:
        break;

    case SM_SVOGrid:
        break;

    case SM_Shadow:
        mScreenQuad->ShowMode = 0;
        mScreenQuad->TempTexture = mShadowMapTexture;
        break;

    case SM_VPLShadow:
        break;

    case SM_GBufferPosition:
        mScreenQuad->ShowMode = 3;
        mScreenQuad->TempTexture = mGBufferPositionTexture;
        break;

    case SM_GBufferNormal:
        mScreenQuad->ShowMode = 0;
        mScreenQuad->TempTexture = mGBufferNormalTexture;
        break;

    case SM_GBufferAlbedo:
        mScreenQuad->ShowMode = 0;
        mScreenQuad->TempTexture = mGBufferAlbedoTexture;
        break;

    case SM_GBufferRPC:
        mScreenQuad->ShowMode = 0;
        mScreenQuad->TempTexture = mGBufferRPCTexture;
        break;

    case SM_RSMPosition:
        mScreenQuad->ShowMode = 1;
        mScreenQuad->TempTextureArray = mRSMPositionTextureArray;
        break;

    case SM_RSMNormal:
        mScreenQuad->ShowMode = 1;
        mScreenQuad->TempTextureArray = mRSMNormalTextureArray;
        break;

    case SM_RSMFlux:
        mScreenQuad->ShowMode = 1;
        mScreenQuad->TempTextureArray = mRSMFluxTextureArray;
        break;

    case SM_DirectLighting:
        mScreenQuad->ShowMode = 0;
        mScreenQuad->TempTexture = mDirectLightingTexture;
        break;

    case SM_IndirectLighting:
        mScreenQuad->ShowMode = 0;
        mScreenQuad->TempTexture = mIndirectLightingTexture;
        break;

    case SM_FilteredIndirectLighting:
        mScreenQuad->ShowMode = 5;
        mScreenQuad->TempTexture = mIndirectLightingTexture;
        break;

    case SM_Final:
        mScreenQuad->ShowMode = 2;
        mScreenQuad->TempTexture = mIndirectLightingTexture;
        mScreenQuad->TempTexture2 = mDirectLightingTexture;
        break;

    default:
        assert(false);
        break;
    }
}
//----------------------------------------------------------------------------
int Visualizer::GetCurVPLSubsetIndex() const
{
    return mCurVPLSubsetIndex;
}
//----------------------------------------------------------------------------
void Visualizer::SetCurVPLSubsetIndex(int value)
{
    mCurVPLSubsetIndex = RTGI_MIN(RTGI_MAX(0, value), mVPLSubsetCount - 1);
}
//----------------------------------------------------------------------------
void Visualizer::SetShowVPL(bool value)
{
    mShowVPL = value;
}
//----------------------------------------------------------------------------
void Visualizer::SetShowVPLSubset(bool value)
{
    mShowVPLSubset = value;
    if( mShowVPLSubset )
    {
        mVPLPointSet->InstanceCount = mVPLPointSet->VPLCount / mVPLSubsetCount;
    }
    else
    {
        mVPLPointSet->InstanceCount = mVPLPointSet->VPLCount;
    }
    mVPLPointSet->ShowVPLSubset = value;
}
//----------------------------------------------------------------------------
void Visualizer::SetShowVPLFluxContrast(bool value)
{
    mVPLPointSet->ShowVPLFluxContrast = value;
}
//----------------------------------------------------------------------------