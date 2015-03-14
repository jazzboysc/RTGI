#include "RefractorResourceRenderer.h"


using namespace RTGI;

//----------------------------------------------------------------------------
RefractorResourceRenderer::RefractorResourceRenderer(GPUDevice* device,
	RenderSet* refractorSet)
:
SubRenderer(device, refractorSet)
{

	mPSB = new PipelineStateBlock();
	mPSB->Flag |= PB_OutputMerger;
	mPSB->OutputMerger.Flag |= OMB_Clear;
	mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
}
//----------------------------------------------------------------------------
RefractorResourceRenderer::~RefractorResourceRenderer()
{
	mPSB = 0;
}
//----------------------------------------------------------------------------
void RefractorResourceRenderer::CreateCausticsResource(RefractorResourceDesc* desc)
{
	assert(desc);

	AddFrameBufferTarget(RTGI_CausticsBuffer_RefractorFrontNormal_Name,
		desc->Width, desc->Height, 0, TT_Texture2D,
		desc->RefractorFrontNormalFormat, desc->RefractorFrontNormalMipmap);
	CreateFrameBuffer(desc->Width, desc->Height, 0, TT_Texture2D);

#ifdef _DEBUG
	GLenum res = glGetError();
	assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void RefractorResourceRenderer::Render(int technique, int pass, Camera* camera)
{
	SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
}
//----------------------------------------------------------------------------