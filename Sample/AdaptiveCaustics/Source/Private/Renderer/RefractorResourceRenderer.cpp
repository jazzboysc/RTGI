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

	AddFrameBufferTarget(RTGI_CausticsBuffer_RefractorFrontAndBackNormal_Name,
		desc->Width, desc->Height, 2, TT_Texture2DArray,
		desc->RefractorFrontNormalFormat, desc->RefractorFrontNormalMipmap);
	CreateFrameBuffer(desc->Width, desc->Height, 2, TT_Texture2DArray);

	// Create material templates.
	mDebugBuffer = new StructuredBuffer();
	auto bufferSize = sizeof(DebugBuffer);
	mDebugBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Copy);

#ifdef _DEBUG
	GLenum res = glGetError();
	assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void RefractorResourceRenderer::Render(int technique, int pass, Camera* camera)
{
	//glEnable(GL_DEPTH_TEST);
	//glDisable(GL_BLEND);
	glDisable(GL_CULL_FACE);

	mDebugBuffer->Bind(0);
	SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
	mDebugBuffer->Bind();

	glEnable(GL_CULL_FACE);

	void* bufferData = mDebugBuffer->Map(BA_Read_Only);
	auto dataPtr = (DebugBuffer*)bufferData;
	mDebugBuffer->Unmap();
	int a = 0;

	auto tex = (Texture2D*)this->GetFrameBufferTextureByName(RTGI_CausticsBuffer_RefractorFrontAndBackNormal_Name);
	tex->GenerateMipmap();
}
//----------------------------------------------------------------------------