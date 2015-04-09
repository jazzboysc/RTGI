#include "CausticMapRenderer.h"

using namespace RTGI;

//----------------------------------------------------------------------------
CausticMapRenderer::CausticMapRenderer(GPUDevice* device,
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
CausticMapRenderer::~CausticMapRenderer()
{
	mPSB = 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
AdaptiveCausticsTraversalInfo::AdaptiveCausticsTraversalInfo()
{}
//----------------------------------------------------------------------------
AdaptiveCausticsTraversalInfo::~AdaptiveCausticsTraversalInfo()
{}
//----------------------------------------------------------------------------
void AdaptiveCausticsTraversalInfo::OnGetShaderConstants()
{
	auto program = this->GetPass(0)->GetShaderProgram();
	program->GetUniformLocation(&ImageUnit0Loc, "data");
	program->GetUniformLocation(&ImageUnit1Loc, "refractorNormal");
}
//----------------------------------------------------------------------------
void AdaptiveCausticsTraversalInfo::OnPreDispatch(unsigned int)
{
	SamplerDesc sampler;
	sampler.MinFilter = FT_Linear_Linear;
	sampler.MagFilter = FT_Linear;
	sampler.WrapS = WT_Clamp;
	sampler.WrapT = WT_Clamp;

	ImageUnit0Loc.SetValue(0);
	ImageUnit1Loc.SetValue(1);
	mCompTexture->BindToImageUnit(0, BufferAccess::BA_Write_Only);
	mNormalTextures->BindToImageUnit(1, BufferAccess::BA_Read_Only, true);
	//mNormalTextures->BindToSampler(1, &sampler);

	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
}
//----------------------------------------------------------------------------
void AdaptiveCausticsTraversalInfo::OnPostDispatch(unsigned int)
{
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void CausticMapRenderer::Initialize(GPUDevice* device,
	ReceiverResourceRenderer* receiverResourceRenderer,
	RefractorResourceRenderer* refractorResourceRenderer,
	Camera* mainCamera)
{

	// Cache temp buffer and textures needed for visualization.
	mReceiverPositionTexture =
		(Texture2D*)receiverResourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_ReceiverPosition_Name);
	mRefractorFrontAndBackNormalTextures =
		(Texture2DArray*)refractorResourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_RefractorFrontAndBackNormal_Name);
}
//----------------------------------------------------------------------------
void CausticMapRenderer::InitializeMinCausticHierarchy(
	GPUDevice* pDevice, VertexBuffer* pVB, int resolution)
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

	mPoints->LoadFromSystemMemory(bufSize/4, causticStartPoints, 4);

	mPoints->CreateDeviceResource(mDevice);

	free(causticStartPoints);
}
//----------------------------------------------------------------------------
void CausticMapRenderer::CreateCausticsResource(CausticsMapDesc* desc)
{
	assert(desc);

	AddFrameBufferTarget(RTGI_CausticsBuffer_CausticMap_Name,
		desc->Width, desc->Height, 0, TT_Texture2D,
		desc->CausticsMapFormat, desc->CausticsMapMipmap);
	CreateFrameBuffer(desc->Width, desc->Height, 0, TT_Texture2D);

	// Create gather voxel fragment list info task.
	ShaderProgramInfo PI_AdaptiveCausticsTraversal;
	PI_AdaptiveCausticsTraversal <<
		"AdaptiveCaustics/AdaptiveCausticsTraversal.comp";

	ComputePass* PassInfo_AdaptiveCausticsTraversal = new ComputePass(
		PI_AdaptiveCausticsTraversal);
	mAdaptiveCausticsTraversalTask = new AdaptiveCausticsTraversalInfo();
	mAdaptiveCausticsTraversalTask->AddPass(PassInfo_AdaptiveCausticsTraversal);
	mAdaptiveCausticsTraversalTask->CreateDeviceResource(mDevice);

	// Create Adaptive caustics traversal shader
	//*
	ShaderProgramInfo PI_Traversal;
	PI_Traversal
		<< "AdaptiveCaustics/TraversalShader_PassDownVertex.vert"
		<< "AdaptiveCaustics/TraversalShader_MaxTraversal.geom"
		<< "AdaptiveCaustics/TraversalShader_DoNothing.frag";
	//*/

	auto mtAdaptiveCausticsTraversal = new MaterialTemplate(
		new Technique({
		new Pass(PI_Traversal) })
		);

	mPoints = new AdaptiveCausticsPointSet(
		new Material(mtAdaptiveCausticsTraversal));

	// First a low-resolution start buffer to begin our hierarchical traversal
	//    (so we need not start from a single photon).
	tbd.genericTraversalStartBuffer = new VertexBuffer();
	// Also initializes the initial data of mStartPoints
	//this->InitializeMinCausticHierarchy(this->mDevice, tbd.genericTraversalStartBuffer, 64);


	// Now we need more generic, large traversal buffers to ping-pong between
	//     for other rendering modes.  Here I allocate an array of these.  Many
	//     render modes need only 1 or 2, those some of the complex ones need
	//     4 or 5.  They probably need not all be the same size, but if I use
	//     different sizes, I have to remember which is which!  (And then they're
	//     not generic!).


	GLsizeiptr traversalBufSz = 2048 * 2048 * sizeof(float)* 4;
	for (int i = 0, j = 5; i < j; ++i)
	{
		tbd.genericTraversalBuffers[i] = new VertexBuffer();
		tbd.genericTraversalBuffers[i]->ReserveMutableDeviceResource(this->mDevice, traversalBufSz, BU_Stream_Copy);
	}

	// Initialize various other OpenGL identifier data
	glGenQueries(1, &mPrimCountQuery);

	mCompTexture = new Texture2D();
	mCompTexture->CreateRenderTarget(mDevice, desc->Width, desc->Height, BF_RGBAF);

#ifdef _DEBUG
	GLenum res = glGetError();
	assert(res == GL_NO_ERROR);
#endif
}
//----------------------------------------------------------------------------
void CausticMapRenderer::Render(int technique, int pass, Camera* camera)
{
	SubRenderer::PreRender(0, 0);
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

	// Setup shortcuts to make code more readable
	auto travBuf = tbd.genericTraversalBuffers;
	auto startBuf = tbd.genericTraversalStartBuffer;

	// We have a specific resolution level to start at (64x64, 2^6 x 2^6, or "level" 6).  
	//   Unfortunately, mipmaps count "0" as the highest resolution, and we count "0" 
	//   as a 1 x 1 image, so we need this computation to get the correct mipmap level 
	//   for our shader.  This is then decremented (because we increase resolution) 
	//   each step as we traverse.
	int decrCounter = (int)(log2(float(mRefractorFrontAndBackNormalTextures->Width)) + 0.01) - startLevel;

	// What resolution is the start of our traversal at?  We'll have 
	//   startResolution*startResolution photon clusters to process during
	//   the first traversal step.
	int startResolution = (int)(pow(2.0f, (float)startLevel) + 0.5);

	// We do ping-ponging back and forth between two temporary vertex buffers
	//   that store our intermediate photons.  To guarantee we always end with
	//   out last pass output to the SAME BUFFER, no matter the # of traversals,
	//   we compute which buffer we should start with.
	int currBuf = 1 - (mMaxTraversalLevel - startLevel) % 2; // Computed so last standard traversal outputs travBuf[0] 

	// A primitive count temporary we'll reuse a lot.
	int primCount = startResolution * startResolution;

	mPoints->NormalTextures = mRefractorFrontAndBackNormalTextures;

	// Don't rasterize these...
	//glEnable(GL_RASTERIZER_DISCARD);
	//glDisable(GL_DEPTH_TEST);
	mAdaptiveCausticsTraversalTask->mCompTexture = mCompTexture;
	mAdaptiveCausticsTraversalTask->mNormalTextures = mRefractorFrontAndBackNormalTextures;
	mAdaptiveCausticsTraversalTask->DispatchCompute(0, 32, 32, 1);
	//glEnable(GL_DEPTH_TEST);

	/*
	traverse,
	stepCount > 0 ? travBuf[1-currBuf] : startBuf,
	travBuf[currBuf],
	data->fbo->deferredCaustic_refractorGeom->GetWidth(),
	decrCounter--,
	stepCount > 0 ? primCount : startResolution * startResolution );
	*/

	/*
	auto deferredBufferWidth = mRefractorFrontAndBackNormalTextures->Width;

	for (int stepCount = 0; stepCount < mMaxTraversalLevel - startLevel; stepCount++)
	{
		int resolution;
		auto inputBuffer = (GLuint)(stepCount > 0 ? travBuf[1 - currBuf] : startBuf)->GetBufferHandle();
		auto outputBuffer = (GLuint)travBuf[currBuf]->GetBufferHandle();
		mPoints->inputBuffer = inputBuffer;
		mPoints->outputBuffer = outputBuffer;
		// Set our output buffer
		glBindBufferBaseNV(GL_TRANSFORM_FEEDBACK_BUFFER_NV, 0, outputBuffer);
		glBeginTransformFeedbackNV(GL_POINTS);
		glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV, mPrimCountQuery);

		// Set our input buffer
		glBindBuffer(GL_ARRAY_BUFFER, inputBuffer);
		glVertexPointer(4, GL_FLOAT, 0, 0/*BUFFER_OFFSET(inputOffsetBytes));

		// Figure out if we're using mipmap data or interpolating between texels at the
		//    full resolution image of the geometry buffers.
		if (decrCounter > 0)
			resolution = deferredBufferWidth / (int)(pow(2.0f, (float)decrCounter) + 0.5);
		else
			resolution = deferredBufferWidth * (int)(pow(2.0f, (float)-decrCounter) + 0.5);

		// How far apart are texels at our current mipmap/traversal level?
		mPoints->deltaHierRes = 0.5f / resolution;
		// What level are we at in the hierarchy?
		mPoints->currHierarchyLevel = (float)decrCounter;

		// Go ahead and draw
		glDrawArrays(GL_POINTS, 0, primCount);

		// Return the number of primitives (photons) output in this pass
		GLint TransformFeedbackPrimCount;
		glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV);
		glEndTransformFeedbackNV();
		glGetQueryObjectiv(mPrimCountQuery, GL_QUERY_RESULT, &TransformFeedbackPrimCount);

		primCount = TransformFeedbackPrimCount;
		currBuf = 1 - currBuf;
	}

	// Disable raterization discards
	glDisable(GL_RASTERIZER_DISCARD);
	*/

	/*

	for (int stepCount = 0; stepCount < *data->param->maxTraversalLevel - startLevel; stepCount++,currBuf=1-currBuf)
	{
		int resolution;

		// Set our output buffer
		glBindBufferBaseNV( GL_TRANSFORM_FEEDBACK_BUFFER_NV, 0, outputBuffer );
		BeginPrimitiveCount( GL_POINTS, data->glID->primCountQuery  );

		// Set our input buffer
		glBindBuffer( GL_ARRAY_BUFFER, inputBuffer );
		glVertexPointer( 4, GL_FLOAT, 0, BUFFER_OFFSET( inputOffsetBytes ) );

		// Figure out if we're using mipmap data or interpolating between texels at the
		//    full resolution image of the geometry buffers.
		if (mipLevel>0)
		resolution = deferredBufferWidth / (int)( pow( 2.0f, (float)mipLevel ) + 0.5 );
		else
		resolution = deferredBufferWidth * (int)( pow( 2.0f, (float)-mipLevel ) + 0.5 );

		// How far apart are texels at our current mipmap/traversal level?
		traverse->SetParameter( "deltaHierRes", 0.5f/resolution );

		// What level are we at in the hierarchy?
		traverse->SetParameter( "currHierarchyLevel", (float)mipLevel );

		// Go ahead and draw
		glDrawArrays( GL_POINTS, 0, inputPrims );

		// Return the number of primitives (photons) output in this pass
		return EndPrimitiveCount( data->glID->primCountQuery  );
	}

	// Disable raterization discards
	glDisable( GL_RASTERIZER_DISCARD_NV );

	// Disable the shader
	FrameBuffer *inputBuf  = data->fbo->deferredCaustic_refractorGeom;
	traverse->DisableTexture( GL_TEXTURE0, inputBuf->GetTextureType( FBO_COLOR0 ) );
	traverse->DisableShader();


	//*/


	//SubRenderer::Render(technique, pass, SRO_FrameBuffer, mPSB, camera);
	SubRenderer::PostRender(0, 0);
}
//----------------------------------------------------------------------------