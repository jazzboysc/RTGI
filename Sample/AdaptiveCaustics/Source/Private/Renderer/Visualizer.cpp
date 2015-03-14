#include "Visualizer.h"

using namespace RTGI;

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
void VisualizerScreenQuad::OnUpdateShaderConstants(int, int)
{
    mShowModeLoc.SetValue(ShowMode);
    mTextureArrayIndexLoc.SetValue(TextureArrayIndex);
    mDimLoc.SetValue(VoxelGridDim);

    SamplerDesc sampler;
    sampler.MinFilter = FT_Nearest;
    sampler.MagFilter = FT_Nearest;
    sampler.WrapS = WT_Clamp;
    sampler.WrapT = WT_Clamp;

    mTempSamplerLoc.SetValue(0);
    if( TempTexture )
    {
        TempTexture->BindToSampler(0, &sampler);
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
    program->GetUniformLocation(&mPositionSamplerLoc, "positionSampler");
    program->GetUniformLocation(&mNormalSamplerLoc, "normalSampler");
    program->GetUniformLocation(&mPositionThresholdLoc, "positionThreshold");
    program->GetUniformLocation(&mNormalThresholdLoc, "normalThreshold");
    program->GetUniformLocation(&mMaxRadianceLoc, "maxRadiance");
    program->GetUniformLocation(&mKernelSizeLoc, "kernelSize");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
Visualizer::Visualizer(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
{
}
//----------------------------------------------------------------------------
Visualizer::~Visualizer()
{

    mReceiverPositionTexture = 0;
    mRefractorFrontNormalTexture = 0;
    mRefractorBackNormalTexture = 0;

    mScreenQuad = 0;
}
//----------------------------------------------------------------------------
void Visualizer::Initialize(GPUDevice* device,
	CausticsResourceRenderer* resourceRenderer,
	Camera* mainCamera)
{
    ShaderProgramInfo visualizerProgramInfo;
    visualizerProgramInfo << "AdaptiveCaustics/TempResult.vert";
    visualizerProgramInfo << "AdaptiveCaustics/TempResult.frag";
    MaterialTemplate* mtScreenQuad =
		new MaterialTemplate(new Technique(
		new Pass(visualizerProgramInfo)));

	// Cache temp buffer and textures needed for visualization.
	mReceiverPositionTexture =
		(Texture2D*)resourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_ReceiverPosition_Name);
	mRefractorFrontNormalTexture =
		(Texture2D*)resourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_RefractorFrontNormal_Name);
	mRefractorBackNormalTexture =
		(Texture2D*)resourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_RefractorBackNormal_Name);

    // Create screen quad.
    Material* material = new Material(mtScreenQuad);
    mScreenQuad = new VisualizerScreenQuad(material);
    mScreenQuad->LoadFromFile("screenquad.ply");
    mScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mScreenQuad->CreateDeviceResource(device);

    SetShowMode(SM_Final);
}
//----------------------------------------------------------------------------
void Visualizer::Render(int technique, int pass)
{
    SubRenderer::Render(technique, pass, SRO_GenericBuffer | SRO_BackBuffer, 
        0, 0);
}
//----------------------------------------------------------------------------
void Visualizer::OnRender(int technique, int pass, Camera*)
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	mScreenQuad->Render(technique, pass);
}
//----------------------------------------------------------------------------
void Visualizer::SetShowMode(ShowMode mode)
{
    mShowMode = mode;
	mScreenQuad->ShowMode = mShowMode;
    switch( mShowMode )
    {
    case SM_Final:
        mScreenQuad->TempTexture = mIndirectLightingTexture;
        mScreenQuad->TempTexture2 = mDirectLightingTexture;
        break;

    default:
        assert(false);
        break;
    }
}
//----------------------------------------------------------------------------