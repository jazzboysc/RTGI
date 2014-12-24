#include "IndirectLightingRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
IndirectLightingScreenQuad::IndirectLightingScreenQuad(Material* material)
    :
    ScreenQuad(material, 0)
{
    BounceSingularity = 60.0f;
}
//----------------------------------------------------------------------------
IndirectLightingScreenQuad::~IndirectLightingScreenQuad()
{
}
//----------------------------------------------------------------------------
void IndirectLightingScreenQuad::OnUpdateShaderConstants(int, int)
{
    mVPLCountLoc.SetValue(VPLCount);
    mBounceSingularityLoc.SetValue(BounceSingularity);
    mGBufferPositionSamplerLoc.SetValue(0);
    mGBufferNormalSamplerLoc.SetValue(1);
    mGBufferAlbedoSamplerLoc.SetValue(2);
}
//----------------------------------------------------------------------------
void IndirectLightingScreenQuad::OnGetShaderConstants()
{
    ShaderProgram* program = mMaterial->GetProgram(0, 0);

    program->GetUniformLocation(&mVPLCountLoc, "VPLCount");
    program->GetUniformLocation(&mBounceSingularityLoc, "BounceSingularity");
    program->GetUniformLocation(&mGBufferPositionSamplerLoc, "GBufferPositionSampler");
    program->GetUniformLocation(&mGBufferNormalSamplerLoc, "GBufferNormalSampler");
    program->GetUniformLocation(&mGBufferAlbedoSamplerLoc, "GBufferAlbedoSampler");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
IndirectLightingRenderer::IndirectLightingRenderer(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
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
void IndirectLightingRenderer::SetInputs(GBufferRenderer* gbuffer, 
    VPLGenerator* vplBuffer)
{
    RendererInputDataView view;
    view.Type = RDT_Texture;
    view.BindingType = BF_BindIndex;
    view.Sampler.MinFilter = FT_Nearest;
    view.Sampler.MagFilter = FT_Nearest;
    view.Sampler.WrapS = WT_Clamp;
    view.Sampler.WrapT = WT_Clamp;

    ClearInputDependency();

    view.BindingSlot = 0;
    AddInputDependency(gbuffer, "Position", &view);

    view.BindingSlot = 1;
    AddInputDependency(gbuffer, "Normal", &view);

    view.BindingSlot = 2;
    AddInputDependency(gbuffer, "Albedo", &view);

    view.Type = RDT_StructuredBuffer;
    view.BindingType = BF_BindIndex;
    view.BindingSlot = 0;
    AddInputDependency(vplBuffer, "VPLBuffer", &view);
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::Initialize(GPUDevice* device, int width, int height,
    Texture::TextureFormat format, int vplCount)
{
    ShaderProgramInfo indirectLightingProgramInfo;
    indirectLightingProgramInfo.VShaderFileName = "BidirectionalVoxelGI/vIndirectLighting.glsl";
    indirectLightingProgramInfo.FShaderFileName = "BidirectionalVoxelGI/fIndirectLighting.glsl";
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

    // Create output.
    AddFrameBufferTarget(RTGI_IndirectLightingRenderer_IndirectLighting_Name,
        width, height, 0, Texture::TT_Texture2D, format);
    CreateFrameBuffer(width, height, 0, Texture::TT_Texture2D);
}
//----------------------------------------------------------------------------
void IndirectLightingRenderer::Render()
{
    SubRenderer::RenderSingle(mIndirectLightingScreenQuad, 0, 0,
        SRO_FrameBuffer, mPSB, 0);
}
//----------------------------------------------------------------------------