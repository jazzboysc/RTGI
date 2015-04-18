#include "Visualizer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VisualizerScreenQuad::VisualizerScreenQuad(Material* material)
    :
    ScreenQuad(material, 0)
{
	ShowMode = Visualizer::eSM_CausticMap;
}
//----------------------------------------------------------------------------
VisualizerScreenQuad::~VisualizerScreenQuad()
{
    DisplayTexture = 0;
}
//----------------------------------------------------------------------------
void VisualizerScreenQuad::OnUpdateShaderConstants(int, int)
{
    mShowModeLoc.SetValue(ShowMode);

    SamplerDesc samplerDesc;
    samplerDesc.MinFilter = FT_Nearest;
    samplerDesc.MagFilter = FT_Nearest;
    samplerDesc.WrapS = WT_Clamp;
    samplerDesc.WrapT = WT_Clamp;

    mTempSamplerLoc.SetValue(0);
	mTempArraySamplerLoc.SetValue(1);
    if( DisplayTexture )
    {
		switch (ShowMode)
		{
		case Visualizer::eSM_GBufferPosition:
			DisplayTexture->BindToSampler(0, &samplerDesc);
			break;
		case Visualizer::eSM_GBufferNormal:
			DisplayTexture->BindToSampler(0, &samplerDesc);
			break;
		case Visualizer::eSM_GBufferAlbedo:
			DisplayTexture->BindToSampler(0, &samplerDesc);
			break;
		case Visualizer::eSM_ReceiverLightSpacePosition:
			DisplayTexture->BindToSampler(0, &samplerDesc);
			break;
		case Visualizer::eSM_RefractorLightSpaceFrontNorm:
			DisplayTexture->BindToSampler(1, &samplerDesc);
			break;
		case Visualizer::eSM_RefractorLightSpaceBackNorm:
			DisplayTexture->BindToSampler(1, &samplerDesc);
			break;
		case Visualizer::eSM_RefractorShadow:
			DisplayTexture->BindToSampler(0, &samplerDesc);
			break;
		case Visualizer::eSM_CausticMap:
			DisplayTexture->BindToSampler(0, &samplerDesc);
			break;
		default:
			assert(false);
			break;
		}
    }
}
//----------------------------------------------------------------------------
void VisualizerScreenQuad::OnGetShaderConstants()
{
    ShaderProgram* program = mMaterial->GetProgram(0, 0);
	program->GetUniformLocation(&mTempSamplerLoc, "tempSampler");
	program->GetUniformLocation(&mTempArraySamplerLoc, "tempSamplerArray");
    program->GetUniformLocation(&mShowModeLoc, "ShowMode");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
Visualizer::Visualizer(GPUDevice* device, RenderSet* renderSet)
    :
    SubRenderer(device, renderSet)
{
	mPSB = new PipelineStateBlock();
	mPSB->Flag |= PB_OutputMerger;
	mPSB->OutputMerger.Flag |= OMB_Clear;
	mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
}
//----------------------------------------------------------------------------
Visualizer::~Visualizer()
{
    mReceiverPositionTexture = 0;
    mRefractorFrontAndBackNormalTextures = 0;

    mScreenQuad = 0;
}
//----------------------------------------------------------------------------
void Visualizer::Initialize(GPUDevice* device,
	GBufferRenderer* gBufferRenderer,
	ReceiverResourceRenderer* receiverResourceRenderer,
	RefractorResourceRenderer* refractorResourceRenderer,
	ShadowMapRenderer* shadowMapRenderer,
	CausticMapRenderer* causticMapRenderer,
	Camera* mainCamera)
{
    ShaderProgramInfo visualizerProgramInfo;
    visualizerProgramInfo << "AdaptiveCaustics/Visualizer.vert";
    visualizerProgramInfo << "AdaptiveCaustics/Visualizer.frag";
    MaterialTemplate* mtScreenQuad =
		new MaterialTemplate(new Technique(
		new Pass(visualizerProgramInfo)));

	// Cache temp buffer and textures needed for visualization.
	mScenePositionTexture =
		(Texture2D*)gBufferRenderer->GetFrameBufferTextureByName(
		RTGI_GBuffer_Position_Name);
	mSceneNormalTexture =
		(Texture2D*)gBufferRenderer->GetFrameBufferTextureByName(
		RTGI_GBuffer_Normal_Name);
	mSceneAlbedoTexture =
		(Texture2D*)gBufferRenderer->GetFrameBufferTextureByName(
		RTGI_GBuffer_Albedo_Name);
	mReceiverPositionTexture =
		(Texture2D*)receiverResourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_ReceiverPosition_Name);
	mRefractorFrontAndBackNormalTextures =
		(Texture2D*)refractorResourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_RefractorFrontAndBackNormal_Name);
	mShadowMapTexture =
		(Texture2D*)shadowMapRenderer->GetFrameBufferTextureByName(
		RTGI_ShadowMapRenderer_ShadowMap_Name);
	mCausticMapTexture =
		(Texture2D*)causticMapRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_CausticMap_Name);
	mCompTexture = causticMapRenderer->mCompTexture;


    // Create screen quad.
    Material* material = new Material(mtScreenQuad);
    mScreenQuad = new VisualizerScreenQuad(material);
	mScreenQuad->LoadFromPLYFile("screenquad.ply");
    mScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mScreenQuad->CreateDeviceResource(device);

	SetShowMode(eSM_CausticMap);
}
//----------------------------------------------------------------------------
void Visualizer::Render(int technique, int pass)
{
    SubRenderer::Render(technique, pass, SRO_GenericBuffer | SRO_BackBuffer, 
        mPSB, 0);
}
//----------------------------------------------------------------------------
void Visualizer::OnRender(int technique, int pass, Camera*)
{
	//glEnable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	mScreenQuad->Render(technique, pass);
}
//----------------------------------------------------------------------------
void Visualizer::SetShowMode(eShowMode mode)
{
    mShowMode = mode;
	mScreenQuad->ShowMode = mShowMode;
    switch( mShowMode )
    {
	case eSM_GBufferPosition:
		mScreenQuad->DisplayTexture = mScenePositionTexture;
		break;
	case eSM_GBufferNormal:
		mScreenQuad->DisplayTexture = mSceneNormalTexture;
		break;
	case eSM_GBufferAlbedo:
		mScreenQuad->DisplayTexture = mSceneAlbedoTexture;
		break;
	case eSM_ReceiverLightSpacePosition:
		mScreenQuad->DisplayTexture = mReceiverPositionTexture;
        break;
	case eSM_RefractorLightSpaceFrontNorm:
		mScreenQuad->DisplayTexture = mRefractorFrontAndBackNormalTextures;
		break;
	case eSM_RefractorLightSpaceBackNorm:
		mScreenQuad->DisplayTexture = mRefractorFrontAndBackNormalTextures;
		break;
	case eSM_RefractorShadow:
		mScreenQuad->DisplayTexture = mShadowMapTexture;
		break;
	case eSM_CausticMap:
		mScreenQuad->DisplayTexture = mCompTexture;
		break;
    default:
        assert(false);
        break;
    }
}
//----------------------------------------------------------------------------