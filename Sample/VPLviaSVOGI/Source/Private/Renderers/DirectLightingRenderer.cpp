#include "DirectLightingRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
DirectLightingScreenQuad::DirectLightingScreenQuad(Material* material)
    :
    ScreenQuad(material, 0)
{
    ShowShadow = true;
}
//----------------------------------------------------------------------------
DirectLightingScreenQuad::~DirectLightingScreenQuad()
{
}
//----------------------------------------------------------------------------
void DirectLightingScreenQuad::OnUpdateShaderConstants(int, int)
{
    mGBufferPositionSamplerLoc.SetValue(0);
    mGBufferNormalSamplerLoc.SetValue(1);
    mGBufferAlbedoSamplerLoc.SetValue(2);
    mShadowMapSamplerLoc.SetValue(3);
    mShowShadow.SetValue(ShowShadow);
}
//----------------------------------------------------------------------------
void DirectLightingScreenQuad::OnGetShaderConstants()
{
    ShaderProgram* program = mMaterial->GetProgram(0, 0);

    program->GetUniformLocation(&mGBufferPositionSamplerLoc, "GBufferPositionSampler");
    program->GetUniformLocation(&mGBufferNormalSamplerLoc, "GBufferNormalSampler");
    program->GetUniformLocation(&mGBufferAlbedoSamplerLoc, "GBufferAlbedoSampler");
    program->GetUniformLocation(&mShadowMapSamplerLoc, "ShadowMapSampler");
    program->GetUniformLocation(&mShowShadow, "ShowShadow");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
DirectLightingRenderer::DirectLightingRenderer(GPUDevice* device, 
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
DirectLightingRenderer::~DirectLightingRenderer()
{
    mPSB = 0;
}
//----------------------------------------------------------------------------
void DirectLightingRenderer::Initialize(GPUDevice* device, int width, 
    int height, BufferFormat format, GBufferRenderer* gbufferRenderer, 
    ShadowMapsGenerator* shadowMapsGenerator)
{
    ShaderProgramInfo directLightingProgramInfo;
    directLightingProgramInfo.VShaderFileName = "VPLviaSVOGI/vDirectLighting.glsl";
    directLightingProgramInfo.FShaderFileName = "VPLviaSVOGI/fDirectLighting.glsl";
    directLightingProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passDirectLighting = new Pass(directLightingProgramInfo);

    Technique* techDirectLighting = new Technique();
    techDirectLighting->AddPass(passDirectLighting);
    MaterialTemplate* mtDirectLighting = new MaterialTemplate();
    mtDirectLighting->AddTechnique(techDirectLighting);

    Material* material = new Material(mtDirectLighting);
    mDirectLightingScreenQuad = new DirectLightingScreenQuad(material);
    mDirectLightingScreenQuad->LoadFromPLYFile("screenquad.ply");
    mDirectLightingScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mDirectLightingScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mDirectLightingScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mDirectLightingScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mDirectLightingScreenQuad->CreateDeviceResource(device);

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

    view.BindingSlot = 3;
    AddInputDependency(shadowMapsGenerator, 
        RTGI_ShadowMapRenderer_ShadowMaps_Name, &view);

    // Create output.
    AddFrameBufferTarget(RTGI_DirectLightingRenderer_DirectLighting_Name, 
        width, height, 0, TT_Texture2D, format);
    CreateFrameBuffer(width, height, 0, TT_Texture2D);
}
//----------------------------------------------------------------------------
void DirectLightingRenderer::Render()
{
    SubRenderer::RenderSingle(mDirectLightingScreenQuad, 0, 0, 
        SRO_FrameBuffer, mPSB, 0);
}
//----------------------------------------------------------------------------
void DirectLightingRenderer::ShowShadow(bool value)
{
    mDirectLightingScreenQuad->ShowShadow = value;
}
//----------------------------------------------------------------------------