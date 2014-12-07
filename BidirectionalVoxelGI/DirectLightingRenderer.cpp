#include "DirectLightingRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
DirectLightingScreenQuad::DirectLightingScreenQuad(Material* material)
    :
    ScreenQuad(material, 0)
{
    LightColor = vec3(0.9686f, 0.9333f, 0.8392f)*0.8f;
}
//----------------------------------------------------------------------------
DirectLightingScreenQuad::~DirectLightingScreenQuad()
{
}
//----------------------------------------------------------------------------
void DirectLightingScreenQuad::OnUpdateShaderConstants(int, int)
{
    glUniform1i(mGBufferPositionSamplerLoc, 0);
    glUniform1i(mGBufferNormalSamplerLoc, 1);
    glUniform1i(mGBufferAlbedoSamplerLoc, 2);
    glUniform1i(mShadowMapSamplerLoc, 3);

    assert(LightProjector);
    if( LightProjector )
    {
        mat4 viewTrans = LightProjector->GetViewTransform();
        glUniformMatrix4fv(mLightProjectorViewLoc, 1, GL_TRUE, viewTrans);

        vec3 lightPosition = LightProjector->GetLocation();
        glUniform3fv(mLightPositionWorldLoc, 1, (GLfloat*)&lightPosition);

        float nearFarPlane[2];
        LightProjector->GetNearFarPlane(nearFarPlane);
        glUniform2fv(mLightProjectorNearFarLoc, 1, nearFarPlane);
    }
    glUniform3fv(mLightColorLoc, 1, (GLfloat*)&LightColor);
}
//----------------------------------------------------------------------------
void DirectLightingScreenQuad::OnGetShaderConstants()
{
    GLuint program = mMaterial->GetProgram(0, 0)->GetProgram();

    mGBufferPositionSamplerLoc = glGetUniformLocation(program, "GBufferPositionSampler");
    mGBufferNormalSamplerLoc = glGetUniformLocation(program, "GBufferNormalSampler");
    mGBufferAlbedoSamplerLoc = glGetUniformLocation(program, "GBufferAlbedoSampler");
    mShadowMapSamplerLoc = glGetUniformLocation(program, "ShadowMapSampler");
    mLightProjectorViewLoc = glGetUniformLocation(program, "LightProjectorView");
    mLightPositionWorldLoc = glGetUniformLocation(program, "LightPositionWorld");
    mLightColorLoc = glGetUniformLocation(program, "LightColor");
    mLightProjectorNearFarLoc = glGetUniformLocation(program, "LightProjectorNearFar");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
DirectLightingRenderer::DirectLightingRenderer(RenderSet* renderSet)
    :
    SubRenderer(renderSet)
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
void DirectLightingRenderer::Initialize(int width, int height, 
    Texture::TextureFormat format, Camera* lightProjector)
{
    ShaderProgramInfo directLightingProgramInfo;
    directLightingProgramInfo.VShaderFileName = "vDirectLighting.glsl";
    directLightingProgramInfo.FShaderFileName = "fDirectLighting.glsl";
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
    mDirectLightingScreenQuad->CreateDeviceResource();
    mDirectLightingScreenQuad->LightProjector = lightProjector;

    // Create output.
    AddFrameBufferTarget("DirectLighting", width, height, 0,
        Texture::TT_Texture2D, format);
    CreateFrameBuffer(width, height, 0, Texture::TT_Texture2D);
}
//----------------------------------------------------------------------------
void DirectLightingRenderer::SetInputs(GBufferRenderer* gbuffer, 
    ShadowMapRenderer* shadowMap)
{
    RendererInputDataView view;
    view.Type = RDT_Texture;
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