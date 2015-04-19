#pragma once

namespace RTGI
{
#define RTGI_DeferredRefractionRenderer_DeferredRefraction_Name "DeferredRefraction"

class DeferredRefractionScreenQuad : public ScreenQuad
{
public:
	DeferredRefractionScreenQuad(Material* material);
	virtual ~DeferredRefractionScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	vec4 RefractorAlbedo;
	vec4 SceneAmbient;
	float RefractionIndex;
	float SplatResolution;
	Texture2DArray* mRefractorNormTex;
	Texture2DArray* mRefractorDepthTex;
	Texture2D* mReceiverColorTex;
	Texture2D* mReceiverDepthTex;

private:
	ShaderUniform mNearFarInfoLoc;
	ShaderUniform mRefractionIndexInfoLoc;
	ShaderUniform mSceneAmbientLoc;
	ShaderUniform mSplatResolutionModifierLoc;
	ShaderUniform mTanEyeFovy2Loc;
	ShaderUniform mRefractorNormLoc;
	ShaderUniform mRefractorDepthLoc;
	ShaderUniform mRefractorAlbedoLoc;
	ShaderUniform mReceiverAlbedoLoc;
	ShaderUniform mReceiverDepthLoc;
};

typedef RefPointer<DeferredRefractionScreenQuad> DeferredRefractionScreenQuadPtr;

class CopyTextureScreenQuad : public ScreenQuad
{
public:
	CopyTextureScreenQuad(Material* material);
	virtual ~CopyTextureScreenQuad();

	// Implement base class interface.
	virtual void OnUpdateShaderConstants(int technique, int pass);
	virtual void OnGetShaderConstants();

	Texture2D* mInputColorTex;
	Texture2D* mInputDepthTex;

private:
	ShaderUniform mInputColorTexLoc;
	ShaderUniform mInputDepthTexLoc;
};
typedef RefPointer<CopyTextureScreenQuad> CopyTextureScreenQuadPtr;


class ReceiverResourceRenderer;
class RefractorResourceRenderer;
class DirectLightingRenderer;
//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 12/05/2014
//----------------------------------------------------------------------------
class DeferredRefractionRenderer : public SubRenderer
{
public:
	DeferredRefractionRenderer(GPUDevice* device, RenderSet* renderSet = 0);
	virtual ~DeferredRefractionRenderer();

	void Initialize(GPUDevice* device, int width, int height,
		BufferFormat format, Camera* camera,
		GBufferRenderer* receiverGBufferRenderer,
		GBufferRenderer* refractorGBufferRenderer,
		ReceiverResourceRenderer* receiverResourceRenderer,
		RefractorResourceRenderer* refractorResourceRenderer,
		RefractorResourceRenderer* refractorNormalRenderer,
		DirectLightingRenderer* directLightingRenderer);

	void Render();

private:
	PipelineStateBlockPtr mPSB;
	DeferredRefractionScreenQuadPtr mDeferredRefractionScreenQuad;
	CopyTextureScreenQuadPtr mCopyTextureScreenQuad;
	FrameBufferPtr mFBOCopy;
};

typedef RefPointer<DeferredRefractionRenderer> DeferredRefractionRendererPtr;

}
