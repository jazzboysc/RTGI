#include "RefractorResourceRendererBack.h"


using namespace RTGI;

//----------------------------------------------------------------------------
RefractorResourceRendererBack::RefractorResourceRendererBack(GPUDevice* device,
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
RefractorResourceRendererBack::~RefractorResourceRendererBack()
{
	mPSB = 0;
}
//----------------------------------------------------------------------------
void RefractorResourceRendererBack::CreateCausticsResource(RefractorResourceDescBack* desc)
{
	assert(desc);

	AddFrameBufferTarget(RTGI_CausticsBuffer_RefractorBackNormal_Name,
		desc->Width, desc->Height, 0, TT_Texture2D,
		desc->RefractorBackNormalFormat, desc->RefractorBackNormalMipmap);
	CreateFrameBuffer(desc->Width, desc->Height, 0, TT_Texture2D);

#ifdef _DEBUG
	GLenum res = glGetError();
	assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void RefractorResourceRendererBack::Render(int technique, int pass, Camera* camera)
{
	glFrontFace(GL_CW);
	SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
	glFrontFace(GL_CCW);

}
//----------------------------------------------------------------------------