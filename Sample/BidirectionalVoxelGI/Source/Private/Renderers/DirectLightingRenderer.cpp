#include "DirectLightingRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
DirectLightingScreenQuad::DirectLightingScreenQuad(Material* material)
    :
    ScreenQuad(material, 0)
{
    LightColor = vec3(0.9686f, 0.9333f, 0.8392f)*0.8f;
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

    assert(LightProjector);
    if( LightProjector )
    {
        mat4 viewTrans = LightProjector->GetViewTransform();
        mLightProjectorViewLoc.SetValue(viewTrans);

        vec3 lightPosition = LightProjector->GetLocation();
        mLightPositionWorldLoc.SetValue(lightPosition);

        float nearFarPlane[2];
        LightProjector->GetNearFarPlane(nearFarPlane);
        mLightProjectorNearFarLoc.SetValue(nearFarPlane);
    }
    mLightColorLoc.SetValue(LightColor);
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
    program->GetUniformLocation(&mLightProjectorViewLoc, "LightProjectorView");
    program->GetUniformLocation(&mLightPositionWorldLoc, "LightPositionWorld");
    program->GetUniformLocation(&mLightColorLoc, "LightColor");
    program->GetUniformLocation(&mLightProjectorNearFarLoc, "LightProjectorNearFar");
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
    int height, TextureFormat format, Camera* lightProjector)
{
    ShaderProgramInfo directLightingProgramInfo;
    directLightingProgramInfo.VShaderFileName = "BidirectionalVoxelGI/vDirectLighting.glsl";
    directLightingProgramInfo.FShaderFileName = "BidirectionalVoxelGI/fDirectLighting.glsl";
    directLightingProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passDirectLighting = new Pass(directLightingProgramInfo);

    Technique* techDirectLighting = new Technique();
    techDirectLighting->AddPass(passDirectLighting);
    MaterialTemplate* mtDirectLighting = new MaterialTemplate();
    mtDirectLighting->AddTechnique(techDirectLighting);

    Material* material = new Material(mtDirectLighting);
    mDirectLightingScreenQuad = new DirectLightingScreenQuad(material);
    mDirectLightingScreenQuad->LoadFromFile("screenquad.ply");
    mDirectLightingScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mDirectLightingScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mDirectLightingScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mDirectLightingScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mDirectLightingScreenQuad->CreateDeviceResource(device);
    mDirectLightingScreenQuad->LightProjector = lightProjector;

    // Create output.
    AddFrameBufferTarget(RTGI_DirectLightingRenderer_DirectLighting_Name, 
        width, height, 0, TT_Texture2D, format);
    CreateFrameBuffer(width, height, 0, TT_Texture2D);
}
//----------------------------------------------------------------------------
void DirectLightingRenderer::SetInputs(GBufferRenderer* gbuffer, 
    ShadowMapRenderer* shadowMap)
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

    view.BindingSlot = 3;
    AddInputDependency(shadowMap, "ShadowMap", &view);
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