#include "ReceiverResourceRenderer.h"


using namespace RTGI;

//----------------------------------------------------------------------------
ReceiverResourceRenderer::ReceiverResourceRenderer(GPUDevice* device,
	RenderSet* receiverSet)
:
SubRenderer(device, receiverSet)
{
	mPSB = new PipelineStateBlock();
	mPSB->Flag |= PB_OutputMerger;
	mPSB->OutputMerger.Flag |= OMB_Clear;
	mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
}
//----------------------------------------------------------------------------
ReceiverResourceRenderer::~ReceiverResourceRenderer()
{
	mPSB = 0;
}
//----------------------------------------------------------------------------
void ReceiverResourceRenderer::CreateCausticsResource(ReceiverResourceDesc* desc)
{
	assert(desc);

	AddFrameBufferTarget(RTGI_CausticsBuffer_ReceiverPosition_Name,
		desc->Width, desc->Height, 0, TT_Texture2D,
		desc->ReceiverPositionFormat, desc->ReceiverPositionMipmap);
	CreateFrameBuffer(desc->Width, desc->Height, 0, TT_Texture2D);

#ifdef _DEBUG
	GLenum res = glGetError();
	assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void ReceiverResourceRenderer::Render(int technique, int pass, Camera* camera)
{
	SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
}
//----------------------------------------------------------------------------