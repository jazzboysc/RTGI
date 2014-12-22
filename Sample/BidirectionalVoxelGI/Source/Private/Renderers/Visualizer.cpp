#include "Visualizer.h"

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
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
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
    TempTextureArray = 0;
    VoxelBuffer = 0;
}
//----------------------------------------------------------------------------
void VisualizerScreenQuad::OnUpdateShaderConstants(int, int)
{
    mShowModeLoc.SetValue(ShowMode);
    mTextureArrayIndexLoc.SetValue(TextureArrayIndex);
    mDimLoc.SetValue(VoxelGridDim);

    mTempSamplerLoc.SetValue(0);
    if( TempTexture )
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TempTexture->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    mTempSampler2Loc.SetValue(1);
    if( TempTexture2 )
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, TempTexture2->GetTexture());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    mTempSamplerArrayLoc.SetValue(2);
    if( TempTextureArray )
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D_ARRAY, TempTextureArray->GetTexture());
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP);
    }

    if( SceneBB )
    {
        mSceneBBMinLoc.SetValue(SceneBB->Min);
        mSceneBBExtensionLoc.SetValue(SceneBB->GetExtension());
    }

    if( VoxelBuffer )
    {
        VoxelBuffer->Bind(0);
    }
}
//----------------------------------------------------------------------------
void VisualizerScreenQuad::OnGetShaderConstants()
{
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
    program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
    program->GetUniformLocation(&mTempSampler2Loc, "tempSampler2");
    program->GetUniformLocation(&mTempSamplerArrayLoc, "tempSamplerArray");
    program->GetUniformLocation(&mShowModeLoc, "ShowMode");
    program->GetUniformLocation(&mTextureArrayIndexLoc, "TextureArrayIndex");
    program->GetUniformLocation(&mSceneBBMinLoc, "SceneBBMin");
    program->GetUniformLocation(&mSceneBBExtensionLoc, "SceneBBExtension");
    program->GetUniformLocation(&mDimLoc, "dim");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
Visualizer::Visualizer(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
{
}
//----------------------------------------------------------------------------
Visualizer::~Visualizer()
{
    mVoxelBuffer = 0;
    mShadowMapTexture = 0;
    mGBufferPositionTexture = 0;
    mGBufferNormalTexture = 0;
    mGBufferAlbedoTexture = 0;
    mRSMPositionTextureArray = 0;
    mRSMNormalTextureArray = 0;
    mRSMFluxTextureArray = 0;
    mDirectLightingTexture = 0;
    mIndirectLightingTexture = 0;
    mScreenQuad = 0;
}
//----------------------------------------------------------------------------
void Visualizer::Initialize(GPUDevice* device, Voxelizer* voxelizer,
    ShadowMapRenderer* shadowMapRenderer, GBufferRenderer* gbufferRenderer, 
    RSMRenderer* rsmRenderer, DirectLightingRenderer* directLightingRenderer,
    IndirectLightingRenderer* indirectLightingRenderer, AABB* sceneBB, 
    int voxelGridDim, int voxelGridLocalGroupDim)
{
    mVoxelGridDim = voxelGridDim;
    mVoxelGridLocalGroupDim = voxelGridLocalGroupDim;
    mGlobalDim = mVoxelGridDim / mVoxelGridLocalGroupDim;

    ShaderProgramInfo visualizerProgramInfo;
    visualizerProgramInfo.VShaderFileName = "BidirectionalVoxelGI/vTempResult.glsl";
    visualizerProgramInfo.FShaderFileName = "BidirectionalVoxelGI/fTempResult.glsl";
    visualizerProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                            ShaderType::ST_Fragment;
    Pass* passScreenQuad = new Pass(visualizerProgramInfo);

    Technique* techScreenQuad = new Technique();
    techScreenQuad->AddPass(passScreenQuad);
    MaterialTemplate* mtScreenQuad = new MaterialTemplate();
    mtScreenQuad->AddTechnique(techScreenQuad);

    mVoxelBuffer = (StructuredBuffer*)voxelizer->GetGenericBufferByName(RTGI_VoxelBuffer_Name);
    mShadowMapTexture = (Texture2D*)shadowMapRenderer->GetFrameBufferTexture(0);
    mGBufferPositionTexture = (Texture2D*)gbufferRenderer->GetFrameBufferTextureByName(RTGI_GBuffer_Position_Name);
    mGBufferNormalTexture = (Texture2D*)gbufferRenderer->GetFrameBufferTextureByName(RTGI_GBuffer_Normal_Name);
    mGBufferAlbedoTexture = (Texture2D*)gbufferRenderer->GetFrameBufferTextureByName(RTGI_GBuffer_Albedo_Name);
    mRSMPositionTextureArray = (Texture2DArray*)rsmRenderer->GetFrameBufferTexture(0);
    mRSMNormalTextureArray = (Texture2DArray*)rsmRenderer->GetFrameBufferTexture(1);
    mRSMFluxTextureArray = (Texture2DArray*)rsmRenderer->GetFrameBufferTexture(2);
    mDirectLightingTexture = (Texture2D*)directLightingRenderer->GetFrameBufferTexture(0);
    mIndirectLightingTexture = (Texture2D*)indirectLightingRenderer->GetFrameBufferTexture(0);

    // Create gather voxel buffer task.
    ShaderProgramInfo gatherVoxelBufferProgramInfo;
    gatherVoxelBufferProgramInfo.CShaderFileName =
        "BidirectionalVoxelGI/cGatherVoxelBuffer.glsl";
    gatherVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passGatherVoxelBuffer = new ComputePass(gatherVoxelBufferProgramInfo);
    mGatherVoxelBufferTask = new GatherVoxelBuffer();
    mGatherVoxelBufferTask->AddPass(passGatherVoxelBuffer);
    mGatherVoxelBufferTask->CreateDeviceResource(device);
    mGatherVoxelBufferTask->SceneBB = sceneBB;

    // Create indirect command buffer.
    int voxelCount = voxelGridDim * voxelGridDim * voxelGridDim;
    int bufferSize = sizeof(unsigned int) * 5 + sizeof(float) * 35
        + voxelCount*sizeof(float) * 4;
    AddGenericBufferTarget("IndirectCommandBuffer", RDT_StructuredBuffer,
        bufferSize, BU_Dynamic_Copy, BF_BindIndex, 1);

    // Create gathered voxel GPU memory allocator counter.
    bufferSize = sizeof(unsigned int);
    AddGenericBufferTarget("GPUMemoryAllocatorCounter",
        RDT_AtomicCounterBuffer, bufferSize, BU_Dynamic_Copy, BF_BindIndex, 0,
        true, 0);

    // Create screen quad.
    Material* material = new Material(mtScreenQuad);
    mScreenQuad = new VisualizerScreenQuad(material);
    mScreenQuad->LoadFromFile("screenquad.ply");
    mScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mScreenQuad->CreateDeviceResource(device);
    mScreenQuad->SceneBB = sceneBB;
    mScreenQuad->VoxelBuffer = mVoxelBuffer;
    mScreenQuad->VoxelGridDim = voxelGridDim;

    SetShowMode(SM_Final);
}
//----------------------------------------------------------------------------
void Visualizer::Render(int technique, int pass)
{
    SubRenderer::Render(technique, pass, SRO_GenericBuffer | SRO_BackBuffer, 0, 0);
}
//----------------------------------------------------------------------------
void Visualizer::OnRender(int technique, int pass, Camera*)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Gather voxel buffer pass.
    mGatherVoxelBufferTask->Dispatch(0, mGlobalDim, mGlobalDim, mGlobalDim);

    mScreenQuad->Render(technique, pass);
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