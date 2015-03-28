#include "IndirectLightingRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
IndirectLightingScreenQuad::IndirectLightingScreenQuad(Material* material)
    :
    ScreenQuad(material, 0)
{
    BounceSingularity = 60.0f;
    VPLVisibilityTest = true;
    UseTC = true;
}
//----------------------------------------------------------------------------
IndirectLightingScreenQuad::~IndirectLightingScreenQuad()
{
}
//----------------------------------------------------------------------------
void IndirectLightingScreenQuad::OnUpdateShaderConstants(int, int)
{
    mVPLCountLoc.SetValue(VPLCount);
    mPatternSizeLoc.SetValue(PatternSize);
    mBounceSingularityLoc.SetValue(BounceSingularity);
    mGBufferPositionSamplerLoc.SetValue(0);
    mGBufferNormalSamplerLoc.SetValue(1);
    mGBufferAlbedoSamplerLoc.SetValue(2);
    mDimLoc.SetValue(VoxelGridDim);
    if( SceneBB )
    {
        mSceneBBCenterLoc.SetValue(SceneBB->GetBoxCenter());
        mSceneBBExtensionLoc.SetValue(SceneBB->GetExtension());
    }
    mVPLVisibilityTestLoc.SetValue(VPLVisibilityTest);
    if( UseTC )
    {
        Application* app = Application::GetInstance();
        mFrameCounterLoc.SetValue((int)app->FrameCounter);
    }
}
//----------------------------------------------------------------------------
void IndirectLightingScreenQuad::OnGetShaderConstants()
{
    ShaderProgram* program = mMaterial->GetProgram(0, 0);

    program->GetUniformLocation(&mVPLCountLoc, "VPLCount");
    program->GetUniformLocation(&mPatternSizeLoc, "PatternSize");
    program->GetUniformLocation(&mBounceSingularityLoc, "BounceSingularity");
    program->GetUniformLocation(&mGBufferPositionSamplerLoc, "GBufferPositionSampler");
    program->GetUniformLocation(&mGBufferNormalSamplerLoc, "GBufferNormalSampler");
    program->GetUniformLocation(&mGBufferAlbedoSamplerLoc, "GBufferAlbedoSampler");
    program->GetUniformLocation(&mSceneBBCenterLoc, "SceneBBCenter");
    program->GetUniformLocation(&mSceneBBExtensionLoc, "SceneBBExtension");
    program->GetUniformLocation(&mDimLoc, "dim");
    program->GetUniformLocation(&mVPLVisibilityTestLoc, "VPLVisibilityTest");
    if( UseTC )
    {
        program->GetUniformLocation(&mFrameCounterLoc, "FrameCounter");
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
IndirectLightingRenderer::IndirectLightingRenderer(GPUDevice* device, 
    RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
{
    mVoxelizerType = Voxelizer::VT_Unknown;
    mUseTC = true;

    mPSB = new PipelineStateBlock();
    mPSB->Flag |= PB_OutputMerger;
    mPSB->OutputMerger.Flag |= OMB_Clear;
    mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
}
//----------------------------------------------------------------------------
IndirectLightingRenderer::~IndirectLightingRenderer()
{
    mPSB = 0;
    mIndirectLightingScreenQuad = 0;

    mVoxelFragmentListBuffer = 0;
    mSVOBuffer = 0;

    IndirectLightingTexture = 0;
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::Initialize(GPUDevice* device, int width, 
    int height, BufferFormat format, int vplCount, int patternSize, 
    AABB* sceneBB, int voxelGridDim, GBufferRenderer* gbufferRenderer,
    VPLGenerator* vplGenerator, Voxelizer* voxelizer, bool useTC)
{
    mVoxelizerType = voxelizer->GetVoxelizerType();
    mUseTC = useTC;

    ShaderProgramInfo indirectLightingProgramInfo;
    indirectLightingProgramInfo.VShaderFileName = 
        "VPLviaSVOGI/vIndirectLighting.glsl";
    if( mVoxelizerType == Voxelizer::VT_Grid )
    {
        indirectLightingProgramInfo.FShaderFileName =
            "VPLviaSVOGI/fGridIndirectLighting.glsl";
    }
    else if( mVoxelizerType == Voxelizer::VT_SVO )
    {
        if( mUseTC )
        {
            indirectLightingProgramInfo.FShaderFileName =
                "VPLviaSVOGI/fSVOIndirectLightingTC.glsl";
        }
        else
        {
            indirectLightingProgramInfo.FShaderFileName =
                "VPLviaSVOGI/fSVOIndirectLighting.glsl";
        }
    }
    else
    {
        assert(false);
    }
    indirectLightingProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                                  ShaderType::ST_Fragment;
    Pass* passIndirectLighting = new Pass(indirectLightingProgramInfo);

    Technique* techIndirectLighting = new Technique();
    techIndirectLighting->AddPass(passIndirectLighting);
    MaterialTemplate* mtIndirectLighting = new MaterialTemplate();
    mtIndirectLighting->AddTechnique(techIndirectLighting);

    Material* material = new Material(mtIndirectLighting);
    mIndirectLightingScreenQuad = new IndirectLightingScreenQuad(material);
    mIndirectLightingScreenQuad->LoadFromPLYFile("screenquad.ply");
    mIndirectLightingScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mIndirectLightingScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mIndirectLightingScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mIndirectLightingScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mIndirectLightingScreenQuad->VPLCount = vplCount;
    mIndirectLightingScreenQuad->PatternSize = patternSize;
    mIndirectLightingScreenQuad->SceneBB = sceneBB;
    mIndirectLightingScreenQuad->VoxelGridDim = voxelGridDim;
    mIndirectLightingScreenQuad->UseTC = useTC;
    mIndirectLightingScreenQuad->CreateDeviceResource(device);

    // Setup inputs.

    RendererInputDataView view;
    view.Type = RDT_Texture;
    view.BindingType = BF_BindIndex;
    view.Sampler.MinFilter = FT_Nearest;
    view.Sampler.MagFilter = FT_Nearest;
    view.Sampler.WrapS = WT_Clamp;
    view.Sampler.WrapT = WT_Clamp;

    ClearInputDependency();

    view.BindingSlot = 0;
    AddInputDependency(gbufferRenderer, RTGI_GBuffer_Position_Name, &view);

    view.BindingSlot = 1;
    AddInputDependency(gbufferRenderer, RTGI_GBuffer_Normal_Name, &view);

    view.BindingSlot = 2;
    AddInputDependency(gbufferRenderer, RTGI_GBuffer_Albedo_Name, &view);

    view.Type = RDT_StructuredBuffer;
    view.BindingType = BF_BindIndex;
    view.BindingSlot = 0;
    AddInputDependency(vplGenerator, RTGI_VPLGenerator_VPLBuffer_Name, &view);

    if( mVoxelizerType == Voxelizer::VT_Grid )
    {
        view.Type = RDT_StructuredBuffer;
        view.BindingType = BF_BindIndex;
        view.BindingSlot = 1;
        AddInputDependency(voxelizer, RTGI_GridVoxelizer_VoxelBuffer_Name,
            &view);
    }
    else if( mVoxelizerType == Voxelizer::VT_SVO )
    {
        // Cache SVO buffers.
        mVoxelFragmentListBuffer = ((SVOVoxelizer*)(
            Voxelizer*)voxelizer)->GetVoxelFragmentListBuffer();
        mSVOBuffer = ((SVOVoxelizer*)(Voxelizer*)voxelizer)->GetSVOBuffer();
    }
    else
    {
        assert(false);
    }

    // Create output.
    if( mUseTC )
    {
        IndirectLightingTexture = new Texture2D();
        IndirectLightingTexture->CreateRenderTarget(mDevice, width, height,
            format, false);
    }
    else
    {
        AddFrameBufferTarget(
            RTGI_IndirectLightingRenderer_IndirectLighting_Name, width,
            height, 0, TT_Texture2D, format);
        CreateFrameBuffer(width, height, 0, TT_Texture2D);
        IndirectLightingTexture = (Texture2D*)GetFrameBufferTextureByName(
            RTGI_IndirectLightingRenderer_IndirectLighting_Name);
    }
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::Render()
{
    if( mVoxelizerType == Voxelizer::VT_SVO )
    {
        mVoxelFragmentListBuffer->Bind(1);
        mSVOBuffer->Bind(3);
    }

    if( mUseTC )
    {
        IndirectLightingTexture->BindToImageUnit(0, BA_Read_Write);
        SubRenderer::RenderSingle(mIndirectLightingScreenQuad, 0, 0,
            SRO_GenericImage, mPSB, 0);
    }
    else
    {
        SubRenderer::RenderSingle(mIndirectLightingScreenQuad, 0, 0,
            SRO_FrameBuffer, mPSB, 0);
    }
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::VPLVisibilityTest(bool value)
{
    mIndirectLightingScreenQuad->VPLVisibilityTest = value;
}
//----------------------------------------------------------------------------