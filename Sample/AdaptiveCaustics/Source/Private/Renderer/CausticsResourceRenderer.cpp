#include "CausticsResourceRenderer.h"


using namespace RTGI;

//----------------------------------------------------------------------------
CausticsResourceRenderer::CausticsResourceRenderer(GPUDevice* device,
	RenderSet* receiverSet, RenderSet* refractorSet)
:
SubRenderer(device)
{
	mReceiverSet = receiverSet;
	mRefractorSet = refractorSet;

	mPSB = new PipelineStateBlock();
	mPSB->Flag |= PB_OutputMerger;
	mPSB->OutputMerger.Flag |= OMB_Clear;
	mPSB->OutputMerger.ClearMask = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
}
//----------------------------------------------------------------------------
CausticsResourceRenderer::~CausticsResourceRenderer()
{
	mPSB = 0;
}
//----------------------------------------------------------------------------
void CausticsResourceRenderer::CreateCausticsResource(CausticsResourceDesc* desc)
{
	assert(desc);

	AddFrameBufferTarget(RTGI_CausticsBuffer_ReceiverPosition_Name,
		desc->Width, desc->Height, 0, TT_Texture2D,
		desc->ReceiverPositionFormat, desc->ReceiverPositionMipmap);
	AddFrameBufferTarget(RTGI_CausticsBuffer_RefractorFrontNormal_Name,
		desc->Width, desc->Height, 0, TT_Texture2D,
		desc->RefractorFrontNormalFormat, desc->RefractorFrontNormalMipmap);
	AddFrameBufferTarget(RTGI_CausticsBuffer_RefractorBackNormal_Name,
		desc->Width, desc->Height, 0, TT_Texture2D,
		desc->RefractorBackNormalFormat, desc->RefractorBackNormalMipmap);
	
	/*
		AddFrameBufferTarget(RTGI_CausticsBuffer_RefractorAlbedo_Name,
		desc->Width, desc->Height, 0, TT_Texture2D,
		desc->RefractorAlbedoFormat, desc->RefractorAlbedoMipmap);
	*/
	CreateFrameBuffer(desc->Width, desc->Height, 0, TT_Texture2D);

#ifdef _DEBUG
	GLenum res = glGetError();
	assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void CausticsResourceRenderer::Render(int technique, int pass, Camera* camera)
{
	SubRenderer::SetRenderSet(mReceiverSet);
	SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
	SubRenderer::SetRenderSet(mRefractorSet);
	SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);

}
//----------------------------------------------------------------------------