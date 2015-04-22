#include "DeferredRefractionRenderer.h"
#include "ReceiverResourceRenderer.h"
#include "RefractorResourceRenderer.h"
#include "DirectLightingRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
DeferredRefractionScreenQuad::DeferredRefractionScreenQuad(Material* material)
:
ScreenQuad(material, 0)
{
	this->RefractionIndex = 1.2f;
	this->SplatResolution = 768;
	this->RefractorAlbedo = vec4(.9f, .9f, .94f, 1.f);
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
	glm::mat4 viewTrans = mCamera->GetViewTransform();
	mViewLoc.SetValue(viewTrans);

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

	mSceneAmbientLoc.SetValue(vec4(.4f, .4f, .4f, 1.f));
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
	program->GetUniformLocation(&mViewLoc, "View");
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
CopyTextureScreenQuad::CopyTextureScreenQuad(Material* material)
:
ScreenQuad(material, 0)
{}
//----------------------------------------------------------------------------
CopyTextureScreenQuad::~CopyTextureScreenQuad()
{}
//----------------------------------------------------------------------------
void CopyTextureScreenQuad::OnUpdateShaderConstants(int, int)
{
	SamplerDesc sampler;
	sampler.MinFilter = FT_Nearest;
	sampler.MagFilter = FT_Nearest;
	sampler.WrapS = WT_Clamp;
	sampler.WrapT = WT_Clamp;

	mInputColorTexLoc.SetValue(0);
	mInputDepthTexLoc.SetValue(1);
	mInputColorTex->BindToSampler(0, &sampler);
	mInputDepthTex->BindToSampler(1, &sampler);
}
//----------------------------------------------------------------------------
void CopyTextureScreenQuad::OnGetShaderConstants()
{
	ShaderProgram* program = mMaterial->GetProgram(0, 0);

	program->GetUniformLocation(&mInputColorTexLoc, "ColorTex");
	program->GetUniformLocation(&mInputDepthTexLoc, "DepthTex");
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
	RefractorResourceRenderer* refractorNormalRenderer,
	DirectLightingRenderer* directLightingRenderer)
{
	ShaderProgramInfo copyTextureProgramInfo;
	copyTextureProgramInfo
		<< "AdaptiveCaustics/CopyTexture/CopyTexture.vert"
		<< "AdaptiveCaustics/CopyTexture/CopyTexture.frag";

	MaterialTemplate* mtCopyTexture =
		new MaterialTemplate(
		new Technique(new Pass(copyTextureProgramInfo)));

	mCopyTextureScreenQuad = new CopyTextureScreenQuad(
		new Material(mtCopyTexture));
	mCopyTextureScreenQuad->LoadFromPLYFile("screenquad.ply");
	mCopyTextureScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
	mCopyTextureScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
	mCopyTextureScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
	mCopyTextureScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
	mCopyTextureScreenQuad->CreateDeviceResource(device);

	mCopyTextureScreenQuad->mInputColorTex =
		(Texture2D*)directLightingRenderer->GetFrameBufferTextureByName(
		RTGI_DirectLightingRenderer_DirectLighting_Name);
	mCopyTextureScreenQuad->mInputDepthTex =
		(Texture2D*)directLightingRenderer->GetDepthTexture();

	ShaderProgramInfo deferredRefractionProgramInfo;
	deferredRefractionProgramInfo
		<< "AdaptiveCaustics/DeferredRefraction/DeferredRefraction.vert"
		<< "AdaptiveCaustics/DeferredRefraction/DeferredRefraction.frag";

	MaterialTemplate* mtDeferredRefraction =
		new MaterialTemplate(
		new Technique(new Pass(deferredRefractionProgramInfo)));

	mDeferredRefractionScreenQuad = new DeferredRefractionScreenQuad(
		new Material(mtDeferredRefraction));
	mDeferredRefractionScreenQuad->LoadFromPLYFile("screenquad.ply");
	mDeferredRefractionScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
	mDeferredRefractionScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
	mDeferredRefractionScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
	mDeferredRefractionScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
	mDeferredRefractionScreenQuad->CreateDeviceResource(device);

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
	AddInputDependency(directLightingRenderer,
		RTGI_DirectLightingRenderer_DirectLighting_Name, &view);

	mDeferredRefractionScreenQuad->mReceiverDepthTex =
		(Texture2D*)receiverGBufferRenderer->GetDepthTexture();

	view.BindingSlot = 2;
	
	AddInputDependency(refractorNormalRenderer, RTGI_CausticsBuffer_RefractorFrontAndBackNormal_Name, &view);

	mDeferredRefractionScreenQuad->mRefractorDepthTex =
		(Texture2DArray*)refractorNormalRenderer->GetDepthTexture();

	// Create output.
	AddFrameBufferTarget(RTGI_DeferredRefractionRenderer_DeferredRefraction_Name,
		width, height, 0, TT_Texture2D, format);
	CreateFrameBuffer(width, height, 0, TT_Texture2D);

	
	Texture* textures[] = {
		(Texture*)GetFrameBufferTextureByName(
		RTGI_DeferredRefractionRenderer_DeferredRefraction_Name) };
	Texture* depthTexture = GetDepthTexture();
	mFBOCopy = new FrameBuffer(mDevice);
	mFBOCopy->SetRenderTargets(1, (Texture**)&textures, depthTexture);
}
//----------------------------------------------------------------------------
void DeferredRefractionRenderer::Render()
{

	// Add deferred refraction
	SubRenderer::RenderSingle(mDeferredRefractionScreenQuad, 0, 0,
		SRO_FrameBuffer, mPSB, mDeferredRefractionScreenQuad->GetCamera());

	// Copy texture
	//glDisable(GL_DEPTH_TEST);
	mFBOCopy->Enable();
	mCopyTextureScreenQuad->Render(0, 0);
	mFBOCopy->Disable();
	//glEnable(GL_DEPTH_TEST);
}
//----------------------------------------------------------------------------