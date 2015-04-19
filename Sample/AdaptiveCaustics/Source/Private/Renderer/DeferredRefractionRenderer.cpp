#include "DeferredRefractionRenderer.h"
#include "ReceiverResourceRenderer.h"
#include "RefractorResourceRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
DeferredRefractionScreenQuad::DeferredRefractionScreenQuad(Material* material)
:
ScreenQuad(material, 0)
{
	this->RefractionIndex = 1.1f;
	this->SplatResolution = 768;
	this->RefractorAlbedo = vec4(1.f, 1.f, 1.f, 1.f);
}
//----------------------------------------------------------------------------
DeferredRefractionScreenQuad::~DeferredRefractionScreenQuad()
{}
//----------------------------------------------------------------------------
void DeferredRefractionScreenQuad::OnUpdateShaderConstants(int, int)
{
	SamplerDesc sampler;
	sampler.MinFilter = FT_Linear;
	sampler.MagFilter = FT_Nearest;
	sampler.WrapS = WT_Clamp;
	sampler.WrapT = WT_Clamp;

	glm::mat4 projTrans = mCamera->GetProjectionTransform();
	mProjLoc.SetValue(projTrans);

	float nearFarPlane[2];
	mCamera->GetNearFarPlane(nearFarPlane);
	mNearFarInfoLoc.SetValue(vec4(
		nearFarPlane[0] * nearFarPlane[1],
		nearFarPlane[1] - nearFarPlane[0],
		nearFarPlane[1] + nearFarPlane[0], nearFarPlane[1]));

	mRefractionIndexInfoLoc.SetValue(vec4(
		1.f / RefractionIndex,
		1.f / (RefractionIndex * RefractionIndex),
		RefractionIndex,
		RefractionIndex * RefractionIndex));

	mSceneAmbientLoc.SetValue(vec4(1.f, 1.f, 1.f, 1.f));
	mSplatResolutionModifierLoc.SetValue(1.f); // depends on the ratio
	mRefractorAlbedoLoc.SetValue(RefractorAlbedo);
	mTanEyeFovy2Loc.SetValue(glm::pi<float>() * mCamera->GetFoV() / 360.0f);

	mReceiverAlbedoLoc.SetValue(0);
	mReceiverDepthLoc.SetValue(1);
	mRefractorNormLoc.SetValue(2);
	mRefractorDepthLoc.SetValue(3);

	mReceiverDepthTex->BindToSampler(1, &sampler);
	mRefractorDepthTex->BindToSampler(3, &sampler);
	
}
//----------------------------------------------------------------------------
void DeferredRefractionScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);

	program->GetUniformLocation(&mNearFarInfoLoc, "NearFarInfo");
	program->GetUniformLocation(&mRefractionIndexInfoLoc, "RefractionIndexInfo");
	program->GetUniformLocation(&mSceneAmbientLoc, "SceneAmbient");
	program->GetUniformLocation(&mSplatResolutionModifierLoc, "SplatResolution");
	program->GetUniformLocation(&mTanEyeFovy2Loc, "TanEyeFovy2");
	program->GetUniformLocation(&mRefractorNormLoc, "RefractorNorm");
	program->GetUniformLocation(&mRefractorDepthLoc, "RefractorDepth");
	program->GetUniformLocation(&mRefractorAlbedoLoc, "RefractorAlbedo");
	program->GetUniformLocation(&mReceiverAlbedoLoc, "ReceiverAlbedo");
	program->GetUniformLocation(&mReceiverDepthLoc, "ReceiverDepth");
	program->GetUniformLocation(&mProjLoc, "Proj");
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
DeferredRefractionRenderer::DeferredRefractionRenderer(GPUDevice* device,
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
DeferredRefractionRenderer::~DeferredRefractionRenderer()
{
	mPSB = 0;
}
//----------------------------------------------------------------------------
void DeferredRefractionRenderer::Initialize(GPUDevice* device, int width,
	int height, BufferFormat format, Camera* camera,
	GBufferRenderer* receiverGBufferRenderer,
	GBufferRenderer* refractorGBufferRenderer,
	ReceiverResourceRenderer* receiverResourceRenderer,
	RefractorResourceRenderer* refractorResourceRenderer,
	RefractorResourceRenderer* refractorNormalRenderer)
{
	ShaderProgramInfo deferredRefractionProgramInfo;
	deferredRefractionProgramInfo
		<< "AdaptiveCaustics/DeferredRefraction/DeferredRefraction.vert"
		<< "AdaptiveCaustics/DeferredRefraction/DeferredRefraction.frag";

	MaterialTemplate* mtDeferredRefraction =
		new MaterialTemplate(
		new Technique(new Pass(deferredRefractionProgramInfo)));

	Material* material = new Material(mtDeferredRefraction);
	mDeferredRefractionScreenQuad = new DeferredRefractionScreenQuad(material);
	mDeferredRefractionScreenQuad->LoadFromPLYFile("screenquad.ply");
	mDeferredRefractionScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
	mDeferredRefractionScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
	mDeferredRefractionScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
	mDeferredRefractionScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
	mDeferredRefractionScreenQuad->CreateDeviceResource(device);
	//mDirectLightingScreenQuad->LightProjector = lightProjector;

	// Setup inputs.
	mDeferredRefractionScreenQuad->SetCamera(camera);

	RendererInputDataView view;
	view.Type = RDT_Texture;
	view.BindingType = BF_BindIndex;
	view.Sampler.MinFilter = FT_Nearest;
	view.Sampler.MagFilter = FT_Nearest;
	view.Sampler.WrapS = WT_Clamp;
	view.Sampler.WrapT = WT_Clamp;

	ClearInputDependency();

	view.BindingSlot = 0;
	AddInputDependency(receiverGBufferRenderer,
		RTGI_GBuffer_Albedo_Name, &view);

	mDeferredRefractionScreenQuad->mReceiverDepthTex =
		(Texture2D*)receiverGBufferRenderer->GetDepthTexture();

	view.BindingSlot = 2;
	
	//AddInputDependency(refractorResourceRenderer, RTGI_CausticsBuffer_RefractorFrontAndBackNormal_Name, &view);
	AddInputDependency(refractorNormalRenderer, RTGI_CausticsBuffer_RefractorFrontAndBackNormal_Name, &view);

	//mDeferredRefractionScreenQuad->mRefractorDepthTex =
	//	(Texture2DArray*)refractorResourceRenderer->GetDepthTexture();
	mDeferredRefractionScreenQuad->mRefractorDepthTex =
		(Texture2DArray*)refractorNormalRenderer->GetDepthTexture();

	// Create output.
	AddFrameBufferTarget(RTGI_DeferredRefractionRenderer_DeferredRefraction_Name,
		width, height, 0, TT_Texture2D, format);
	CreateFrameBuffer(width, height, 0, TT_Texture2D);
}
//----------------------------------------------------------------------------
void DeferredRefractionRenderer::Render()
{
	SubRenderer::RenderSingle(mDeferredRefractionScreenQuad, 0, 0,
		SRO_FrameBuffer, mPSB, mDeferredRefractionScreenQuad->GetCamera());
}
//----------------------------------------------------------------------------