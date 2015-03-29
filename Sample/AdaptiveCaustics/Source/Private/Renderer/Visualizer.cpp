#include "Visualizer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
VisualizerScreenQuad::VisualizerScreenQuad(Material* material)
    :
    ScreenQuad(material, 0)
{
    ShowMode = 0;
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
		case Visualizer::eSM_ReceiverLightSpacePosition:
			DisplayTexture->BindToSampler(0, &samplerDesc);
			break;
		case Visualizer::eSM_RefractorLightSpaceFrontNorm:
			DisplayTexture->BindToSampler(1, &samplerDesc);
			break;
		case Visualizer::eSM_RefractorLightSpaceBackNorm:
			DisplayTexture->BindToSampler(1, &samplerDesc);
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
    mRefractorFrontNormalTexture = 0;
    mRefractorBackNormalTexture = 0;

    mScreenQuad = 0;
}
//----------------------------------------------------------------------------
void Visualizer::Initialize(GPUDevice* device,
	ReceiverResourceRenderer* receiverResourceRenderer,
	RefractorResourceRenderer* refractorResourceRenderer,
	RefractorResourceRendererBack* refractorResourceRendererBack,
	Camera* mainCamera)
{
    ShaderProgramInfo visualizerProgramInfo;
    visualizerProgramInfo << "AdaptiveCaustics/Visualizer.vert";
    visualizerProgramInfo << "AdaptiveCaustics/Visualizer.frag";
    MaterialTemplate* mtScreenQuad =
		new MaterialTemplate(new Technique(
		new Pass(visualizerProgramInfo)));

	// Cache temp buffer and textures needed for visualization.
	mReceiverPositionTexture =
		(Texture2D*)receiverResourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_ReceiverPosition_Name);
	mRefractorFrontNormalTexture =
		(Texture2D*)refractorResourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_RefractorFrontNormal_Name);
// 	mRefractorBackNormalTexture =
// 		(Texture2D*)refractorResourceRendererBack->GetFrameBufferTextureByName(
// 		RTGI_CausticsBuffer_RefractorBackNormal_Name);

    // Create screen quad.
    Material* material = new Material(mtScreenQuad);
    mScreenQuad = new VisualizerScreenQuad(material);
	mScreenQuad->LoadFromPLYFile("screenquad.ply");
    mScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mScreenQuad->CreateDeviceResource(device);

	SetShowMode(eSM_ReceiverLightSpacePosition);
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
	mScreenQuad->Render(technique, pass);
}
//----------------------------------------------------------------------------
void Visualizer::SetShowMode(eShowMode mode)
{
    mShowMode = mode;
	mScreenQuad->ShowMode = mShowMode;
    switch( mShowMode )
    {
	case eSM_ReceiverLightSpacePosition:
		mScreenQuad->DisplayTexture = mReceiverPositionTexture;
        break;
	case eSM_RefractorLightSpaceFrontNorm:
		mScreenQuad->DisplayTexture = mRefractorFrontNormalTexture;
		break;
	case eSM_RefractorLightSpaceBackNorm:
		mScreenQuad->DisplayTexture = mRefractorFrontNormalTexture;
		break;

    default:
        assert(false);
        break;
    }
}
//----------------------------------------------------------------------------