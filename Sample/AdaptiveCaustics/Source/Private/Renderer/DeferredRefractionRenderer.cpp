#include "DeferredRefractionRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
DeferredRefractionScreenQuad::DeferredRefractionScreenQuad(Material* material)
:
ScreenQuad(material, 0)
{}
//----------------------------------------------------------------------------
DeferredRefractionScreenQuad::~DeferredRefractionScreenQuad()
{}
//----------------------------------------------------------------------------
void DeferredRefractionScreenQuad::OnUpdateShaderConstants(int, int)
{
	glm::mat4 projTrans = mCamera->GetProjectionTransform();
	mProjLoc.SetValue(projTrans);

	float nearFarPlane[2];
	mCamera->GetNearFarPlane(nearFarPlane);
	mNearFarInfoLoc.SetValue(vec4(
		nearFarPlane[0] * nearFarPlane[1],
		nearFarPlane[1] - nearFarPlane[0],
		nearFarPlane[1] + nearFarPlane[0], nearFarPlane[1]));


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
	program->GetUniformLocation(&mRefractorColorLoc, "RefractorColor");
	program->GetUniformLocation(&mReceiverColorLoc, "ReceiverAlbedo");
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
	int height, BufferFormat format, GBufferRenderer* gbufferRenderer,
	ShadowMapRenderer* shadowMapRenderer)
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
	AddInputDependency(shadowMapRenderer, RTGI_ShadowMapRenderer_ShadowMap_Name, &view);

	// Create output.
	AddFrameBufferTarget(RTGI_DeferredRefractionRenderer_DeferredRefraction_Name,
		width, height, 0, TT_Texture2D, format);
	CreateFrameBuffer(width, height, 0, TT_Texture2D);
}
//----------------------------------------------------------------------------
void DeferredRefractionRenderer::Render()
{
	SubRenderer::RenderSingle(mDeferredRefractionScreenQuad, 0, 0,
		SRO_FrameBuffer, mPSB, 0);
}
//----------------------------------------------------------------------------