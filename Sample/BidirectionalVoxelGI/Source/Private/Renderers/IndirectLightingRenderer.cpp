#include "IndirectLightingRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
IndirectLightingScreenQuad::IndirectLightingScreenQuad(Material* material)
    :
    ScreenQuad(material, 0)
{
    BounceSingularity = 60.0f;
    VPLVisibilityTest = true;
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
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
IndirectLightingRenderer::IndirectLightingRenderer(GPUDevice* device, 
    RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
{
    mPSB = new PipelineStateBlock();
    mPSB->Flag |= PB_OutputMerger;
    mPSB->OutputMerger.Flag |= OMB_Clear;
    mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
}
//----------------------------------------------------------------------------
IndirectLightingRenderer::~IndirectLightingRenderer()
{
    mPSB = 0;
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::Initialize(GPUDevice* device, int width, 
    int height, BufferFormat format, int vplCount, int patternSize, 
    AABB* sceneBB, int voxelGridDim, GBufferRenderer* gbufferRenderer,
    VPLGenerator* vplGenerator, Voxelizer* voxelizer)
{
    VoxelizerType vt = voxelizer->GetVoxelizerType();

    ShaderProgramInfo indirectLightingProgramInfo;
    indirectLightingProgramInfo.VShaderFileName = 
        "BidirectionalVoxelGI/vIndirectLighting.glsl";
    if( vt == VT_Grid )
    {
        indirectLightingProgramInfo.FShaderFileName =
            "BidirectionalVoxelGI/fGridIndirectLighting.glsl";
    }
    else if( vt == VT_SVO )
    {
        indirectLightingProgramInfo.FShaderFileName =
            "BidirectionalVoxelGI/fSVOIndirectLighting.glsl";
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
    mIndirectLightingScreenQuad->LoadFromFile("screenquad.ply");
    mIndirectLightingScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mIndirectLightingScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mIndirectLightingScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mIndirectLightingScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mIndirectLightingScreenQuad->CreateDeviceResource(device);
    mIndirectLightingScreenQuad->VPLCount = vplCount;
    mIndirectLightingScreenQuad->PatternSize = patternSize;
    mIndirectLightingScreenQuad->SceneBB = sceneBB;
    mIndirectLightingScreenQuad->VoxelGridDim = voxelGridDim;

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

    if( vt == VT_Grid )
    {
        view.Type = RDT_StructuredBuffer;
        view.BindingType = BF_BindIndex;
        view.BindingSlot = 1;
        AddInputDependency(voxelizer, RTGI_GridVoxelizer_VoxelBuffer_Name,
            &view);
    }
    else if( vt == VT_SVO )
    {
        assert(false);
    }
    else
    {
        assert(false);
    }

    // Create output.
    AddFrameBufferTarget(RTGI_IndirectLightingRenderer_IndirectLighting_Name,
        width, height, 0, TT_Texture2D, format);
    CreateFrameBuffer(width, height, 0, TT_Texture2D);
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::Render()
{
    SubRenderer::RenderSingle(mIndirectLightingScreenQuad, 0, 0,
        SRO_FrameBuffer, mPSB, 0);
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::VPLVisibilityTest(bool value)
{
    mIndirectLightingScreenQuad->VPLVisibilityTest = value;
}
//----------------------------------------------------------------------------