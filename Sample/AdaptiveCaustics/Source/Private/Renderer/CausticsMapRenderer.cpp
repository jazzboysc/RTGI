#include "CausticsMapRenderer.h"


using namespace RTGI;

//----------------------------------------------------------------------------
CausticsMapRenderer::CausticsMapRenderer(GPUDevice* device,
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
CausticsMapRenderer::~CausticsMapRenderer()
{
	mPSB = 0;
}

void InitializeMinCausticHierarchy(GPUDevice* pDevice, VertexBuffer* pVB, int resolution)
{
	size_t bufSize = resolution * resolution * sizeof(fvec4);
	float *causticStartPoints = (float *)malloc(bufSize);
	for (int i = 0, j = resolution * resolution; i < j; ++i)
	{
		// 3D buffer: res * res * 4
		int x = i % resolution;
		int y = i / resolution;
		causticStartPoints[4 * i + 0] = x / (float)resolution;
		causticStartPoints[4 * i + 1] = y / (float)resolution;
		causticStartPoints[4 * i + 2] = 0;
		causticStartPoints[4 * i + 3] = 0;
	}
	pVB->LoadFromSystemMemory(pDevice, bufSize, causticStartPoints, RTGI::BufferUsage::BU_Static_Draw);
	free(causticStartPoints);
}
//----------------------------------------------------------------------------
void CausticsMapRenderer::CreateCausticsResource(CausticsMapDesc* desc)
{
	assert(desc);

	AddFrameBufferTarget(RTGI_CausticsBuffer_CausticsMap_Name,
		desc->Width, desc->Height, 0, TT_Texture2D,
		desc->CausticsMapFormat, desc->CausticsMapMipmap);
	CreateFrameBuffer(desc->Width, desc->Height, 0, TT_Texture2D);

	// First a low-resolution start buffer to begin our hierarchical traversal
	//    (so we need not start from a single photon).
	tbd.genericTraversalStartBuffer = new VertexBuffer();
	InitializeMinCausticHierarchy(this->mDevice, tbd.genericTraversalStartBuffer, 64);


	// Now we need more generic, large traversal buffers to ping-pong between
	//     for other rendering modes.  Here I allocate an array of these.  Many
	//     render modes need only 1 or 2, those some of the complex ones need
	//     4 or 5.  They probably need not all be the same size, but if I use
	//     different sizes, I have to remember which is which!  (And then they're
	//     not generic!).
	GLsizeiptr traversalBufSz = 2048 * 2048 * sizeof(float)* 4;
	for (int i = 0, j = 5; i < j; ++i)
	{
		tbd.genericTraversalBuffer[i] = new VertexBuffer();
		tbd.genericTraversalBuffer[i]->ReserveMutableDeviceResource(this->mDevice, traversalBufSz, BU_Stream_Copy);
	}

#ifdef _DEBUG
	GLenum res = glGetError();
	assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void CausticsMapRenderer::Render(int technique, int pass, Camera* camera)
{
	// The actual hierarchical traversal is given in code in utilRoutines.cpp.  This
	//    code traverses our photon hierarchy and outputs the correct photons as 
	//    splats into the current buffer (the causticMap[lightNum] enabled above)
	glBlendFunc(GL_ONE, GL_ONE);
	int startLevel = 6;  // The starting traversal level (2^6 = 64x64 photons)

// 	HierarchicalTransformFeedbackTraversalWithBatching(
//		data->shader->adaptCaustic_traverse,
// 		data->shader->adaptCaustic_splat,
// 		startLevel,
// 		lightNum);

	/*
	// Setup shortcuts to make code more readable
	GLuint *travBuf = data->vbo->genericTraversalBuffer;
	GLuint startBuf = data->vbo->genericTraversalStartBuffer;

	// We have a specific resolutionlevel to start at (64x64, 2^6 x 2^6, or "level" 6).  
	//   Unfortunately, mipmaps count "0" as the highest resolution, and we count "0" 
	//   as a 1 x 1 image, so we need this computation to get the correct mipmap level 
	//   for our shader.  This is then decremented (because we increase resolution) 
	//   each step as we traverse.
	int decrCounter = (int)(log2(data->fbo->deferredCaustic_refractorGeom->GetWidth()) + 0.01) - startLevel;

	// What resolution is the start of our traversal at?  We'll have 
	//   startResolution*startResolution photon clusters to process during
	//   the first traversal step.
	int startResolution = (int)(pow(2.0f, (float)startLevel) + 0.5);

	// We do ping-ponging back and forth between two temporary vertex buffers
	//   that store our intermediate photons.  To guarantee we always end with
	//   out last pass output to the SAME BUFFER, no matter the # of traversals,
	//   we compute which buffer we should start with.
	int currBuf = 1 - (*data->param->maxTraversalLevel - startLevel) % 2; // Computed so last standard traversal outputs travBuf[0] 

	// A primitive count temporary we'll reuse a lot.
	int primCount = 0;
	//*/


	SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
}
//----------------------------------------------------------------------------