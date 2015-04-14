#include "CausticMapRenderer.h"

using namespace RTGI;
#define START_LOD 6.0f
#define START_KERNEL_WIDTH (uint)(pow(2.0f, (float)START_LOD) + 0.5f)
#define START_KERNEL_SIZE START_KERNEL_WIDTH * START_KERNEL_WIDTH
#define KERNEL_WIDTH(lod) (uint)(pow(2.0f, (float)lod) + 0.5f)
#define KERNEL_SIZE(lod) KERNEL_WIDTH(lod) * KERNEL_WIDTH(lod)

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

	DebugMipmapLevel = 0;
	DrawDebugMipmap = false;
}
//----------------------------------------------------------------------------
CausticMapRenderer::~CausticMapRenderer()
{
	mPSB = 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
AdaptiveCausticsTaskInfo::AdaptiveCausticsTaskInfo()
{}
//----------------------------------------------------------------------------
AdaptiveCausticsTaskInfo::~AdaptiveCausticsTaskInfo()
{}
//----------------------------------------------------------------------------
void AdaptiveCausticsTaskInfo::OnGetShaderConstants()
{
	auto program = this->GetPass(0)->GetShaderProgram();
	program->GetUniformLocation(&ImageUnit0Loc, "data");
	program->GetUniformLocation(&ImageUnit1Loc, "refractorNormal");
}
//----------------------------------------------------------------------------
void AdaptiveCausticsTaskInfo::OnPreDispatch(unsigned int pass)
{
	SamplerDesc sampler;
	sampler.MinFilter = FT_Linear;
	sampler.MagFilter = FT_Nearest;
	sampler.WrapS = WT_Clamp;
	sampler.WrapT = WT_Clamp;

	if (pass == 0)
	{
		ImageUnit0Loc.SetValue(0);
		ImageUnit1Loc.SetValue(1);
		mCompTexture->BindToImageUnit(0, BufferAccess::BA_Write_Only);
		//mNormalTextures->BindToImageUnit(1, BufferAccess::BA_Read_Only, true);
		mNormalTextures->BindToSampler(1, &sampler);
	}

	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
}
//----------------------------------------------------------------------------
void AdaptiveCausticsTaskInfo::OnPostDispatch(unsigned int pass)
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
	GPUDevice* pDevice, AdaptiveCausticsTaskInfo* pVB, int resolution)
{
	size_t bufSize = resolution * resolution * sizeof(vec4);
	vec4* causticStartPoints = (vec4*) malloc(bufSize);
	for (int i = 0, j = resolution * resolution; i < j; ++i)
	{
		// 3D buffer: res * res * 4
		int x = i % resolution;
		int y = i / resolution;
		causticStartPoints[i] = fvec4(x / (float)resolution, y / (float)resolution, 0, 0);
		//causticStartPoints[i] = fvec4(1, 0, 0, 0);
	}

	mTraversalTask->mACMBuffer->Bind(0);
	mTraversalTask->mACMBuffer->UpdateSubData(0, sizeof(ACMBuffer), bufSize, causticStartPoints);

	free(causticStartPoints);

	//pVB->LoadFromSystemMemory(pDevice, bufSize, causticStartPoints, RTGI::BufferUsage::BU_Static_Draw);

	//mPoints->LoadFromSystemMemory(bufSize/4, causticStartPoints, 4);

	//mPoints->CreateDeviceResource(mDevice);

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
	PI_AdaptiveCausticsTraversal
		<< "AdaptiveCaustics/AdaptiveCausticsTraversal.comp";

	ShaderProgramInfo PI_AdaptiveCausticsPostTraversalProcess;
	PI_AdaptiveCausticsPostTraversalProcess
	<< "AdaptiveCaustics/AdaptiveCausticsPostTraversalProcess.comp";


	ComputePass* PassInfo_AdaptiveCausticsTraversal = new ComputePass(
		PI_AdaptiveCausticsTraversal);
	ComputePass* PassInfo_AdaptivePostTraversalProcess = new ComputePass(
		PI_AdaptiveCausticsPostTraversalProcess);
	mTraversalTask = new AdaptiveCausticsTaskInfo();
	mTraversalTask->AddPass(PassInfo_AdaptiveCausticsTraversal);
	mTraversalTask->AddPass(PassInfo_AdaptivePostTraversalProcess);
	mTraversalTask->CreateDeviceResource(mDevice);

	ShaderProgramInfo PI_AdaptiveCausticsDebugDraw;
	PI_AdaptiveCausticsDebugDraw <<
		"AdaptiveCaustics/AdaptiveCausticsDrawDebug.comp";

	ComputePass* PassInfo_AdaptiveCausticsDebugDraw = new ComputePass(
		PI_AdaptiveCausticsDebugDraw);
	mDebugDrawTask = new AdaptiveCausticsTaskInfo();
	mDebugDrawTask->AddPass(PassInfo_AdaptiveCausticsDebugDraw);
	mDebugDrawTask->CreateDeviceResource(mDevice);

	// Now we need more generic, large traversal buffers to ping-pong between
	//     for other rendering modes.  Here I allocate an array of these.  Many
	//     render modes need only 1 or 2, those some of the complex ones need
	//     4 or 5.  They probably need not all be the same size, but if I use
	//     different sizes, I have to remember which is which!  (And then they're
	//     not generic!).

	mCompTexture = new Texture2D();
	mCompTexture->CreateRenderTarget(mDevice, desc->Width, desc->Height, BF_RGBAF);

	// Clear without drawing
	Texture2D* textures[] = { mCompTexture };
	mFBOClear = new FrameBuffer(mDevice);
	mFBOClear->SetRenderTargets(1, (Texture**)&mCompTexture);

	mTraversalTask->mAtomicCounterBuffer = new AtomicCounterBuffer();
#ifdef DEBUG_CAUSTCIS
	mTraversalTask->mAtomicCounterBuffer->ReserveMutableDeviceResource(mDevice,
		sizeof(ACMAtomicCounter), BU_Dynamic_Copy);
#else
	mTraversalTask->mAtomicCounterBuffer->ReserveMutableDeviceResource(mDevice,
		sizeof(ACMAtomicCounter), BU_Dynamic_Copy);
#endif

	// Initialize atomic counter
	mTraversalTask->AtomicCounterCache.writeCount = START_KERNEL_SIZE / 4;
	mTraversalTask->AtomicCounterCache.temp = 0;
	mTraversalTask->mAtomicCounterBuffer->UpdateSubData(0, 0, sizeof(ACMAtomicCounter),
		(void*)&mTraversalTask->AtomicCounterCache);
	//mTraversalTask->mAtomicCounterBuffer->Clear(BIF_R32UI, BF_R32UI, BCT_Unsigned_Int,
	//	counterData0);
	// Storage buffer
	mTraversalTask->mACMBuffer = new StructuredBuffer();
	auto bufferSize = sizeof(ACMBuffer) + KERNEL_SIZE(11) * sizeof(vec4);
	mTraversalTask->mACMBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Stream_Copy);
	InitializeMinCausticHierarchy(mDevice, mTraversalTask, START_KERNEL_WIDTH);

	// Shared Command buffer
	mTraversalTask->mACMSharedCommandBuffer = new StructuredBuffer();
	auto bufferSize1 = sizeof(ACMSharedCommandBuffer);
	mTraversalTask->mACMSharedCommandBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Copy);

	// Shared command buffer for debug draw
	mDebugDrawTask->mACMSharedCommandBuffer = mTraversalTask->mACMSharedCommandBuffer;

#ifdef _DEBUG
	GLenum res = glGetError();
	assert(res == GL_NO_ERROR);
#endif
}

//#define DEBUG_CAUSTCIS


//----------------------------------------------------------------------------
void CausticMapRenderer::Render(int technique, int pass, Camera* camera)
{
	SubRenderer::PreRender(0, 0);
	
	mFBOClear->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mFBOClear->Disable();
	
	// Setup input and output
	mTraversalTask->mCompTexture = mCompTexture;
	mTraversalTask->mNormalTextures = mRefractorFrontAndBackNormalTextures;

	if (DrawDebugMipmap)
	{
		mDebugDrawTask->mCompTexture = mCompTexture;
		mDebugDrawTask->mNormalTextures = mRefractorFrontAndBackNormalTextures;

		// Add in debug parameters
		mDebugDrawTask->mACMSharedCommandBuffer->Bind();
		auto uniformData3 = (ACMSharedCommandBuffer*)mDebugDrawTask->mACMSharedCommandBuffer->Map(BA_Write_Only);
		assert(uniformData3);
		if (uniformData3)
		{
			uniformData3->mipmapLevel = DebugMipmapLevel;
			uniformData3->width = mCompTexture->Width;
			uniformData3->height = mCompTexture->Height;
		}
		mDebugDrawTask->mACMSharedCommandBuffer->Unmap();

		// Draw debug mipmap normal
		mTraversalTask->mACMSharedCommandBuffer->Bind(0);
		mTraversalTask->mACMBuffer->Bind(1);
		mDebugDrawTask->DispatchCompute(0, mCompTexture->Width, mCompTexture->Height, 1);
	}

	// Reset atomic counters.
#ifdef DEBUG_CAUSTCIS
	auto counterData0 =
		(ACMAtomicCounter*)mTraversalTask->mAtomicCounterBuffer->Map(BA_Read_Write);
	assert(counterData0);
	if (counterData0)
	{
		mTraversalTask->AtomicCounterCache = *counterData0;
		counterData0->writeCount = START_KERNEL_SIZE / 4;
		counterData0->temp = 0;
	}
	mTraversalTask->mAtomicCounterBuffer->Unmap();
#else
	unsigned int counterData0[2];
	counterData0[0] = 0;
	counterData0[1] = 0;
	mTraversalTask->mAtomicCounterBuffer->Clear(BIF_R32UI, BF_R32UI, BCT_Unsigned_Int,
		counterData0);
#endif

	// Initialize uniform buffer
#ifdef DEBUG_CAUSTCIS
	auto uniformData = (ACMUniformBuffer*)mTraversalTask->mACMUniformBuffer->Map(BA_Read_Write);
#else
	mTraversalTask->mACMSharedCommandBuffer->Bind();
	auto uniformData = (ACMSharedCommandBuffer*)mTraversalTask->mACMSharedCommandBuffer->Map(BA_Write_Only);
#endif
	assert(uniformData);
	if (uniformData)
	{
		uniformData->mipmapLevel = (int)(log2(float(mRefractorFrontAndBackNormalTextures->Width)) + 0.01) - (uint)START_LOD;
		uniformData->readOffset = 0;
		uniformData->readCount = START_KERNEL_SIZE;
		uniformData->writeOffset = START_KERNEL_SIZE;
		uniformData->width = mCompTexture->Width;
		uniformData->height = mCompTexture->Height;
		uniformData->deltaX = 0.5f / (float)START_KERNEL_WIDTH;
		uniformData->deltaY = 0.5f / (float)START_KERNEL_WIDTH;
	}
	mTraversalTask->mACMSharedCommandBuffer->Unmap();

	// Initialize uniform buffer cache
	mTraversalTask->UniformCache.mipmapLevel = 0;
	mTraversalTask->UniformCache.readOffset = 0;
	mTraversalTask->UniformCache.readCount = START_KERNEL_SIZE;
	mTraversalTask->UniformCache.writeOffset = START_KERNEL_SIZE / 4; // Every vertex plits into 4
	mTraversalTask->UniformCache.width = mCompTexture->Width;
	mTraversalTask->UniformCache.height = mCompTexture->Height;
	mTraversalTask->UniformCache.deltaX = 0.5f / (float)START_KERNEL_WIDTH;
	mTraversalTask->UniformCache.deltaY = 0.5f / (float)START_KERNEL_WIDTH;

	// Initialize atomic counter
	mTraversalTask->AtomicCounterCache.writeCount = START_KERNEL_SIZE / 4;
	mTraversalTask->AtomicCounterCache.temp = 0;

	/*
	int result[6];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &result[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &result[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &result[2]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &result[3]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 0, &result[4]);
	*/
	uint currentLOD = (uint)START_LOD;  // The starting traversal level (2^6 = 64x64 photons)
	int numLevels = (int)(log2(float(mRefractorFrontAndBackNormalTextures->Width)) + 0.01) - currentLOD;

	for (int i = 0; i < numLevels + 1; ++i, ++currentLOD)
	{
		// We have a specific resolution level to start at (64x64, 2^6 x 2^6, or "level" 6).  
		//   Unfortunately, mipmaps count "0" as the highest resolution, and we count "0" 
		//   as a 1 x 1 image, so we need this computation to get the correct mipmap level 
		//   for our shader.  This is then decremented (because we increase resolution) 
		//   each step as we traverse.
		int glMipmapLevel = (int)(log2(float(mRefractorFrontAndBackNormalTextures->Width)) + 0.01) - currentLOD;

		// What resolution is the start of our traversal at?  We'll have 
		//   startResolution*startResolution photon clusters to process during
		//   the first traversal step.
		int currentResolution = KERNEL_WIDTH(currentLOD);

		// Reset atomic counters.
#ifdef DEBUG_CAUSTCIS
		mTraversalTask->mAtomicCounterBuffer->Bind(0);
		auto counterData = (ACMAtomicCounter*)mTraversalTask->mAtomicCounterBuffer->Map(BA_Read_Write);
		assert(counterData);
		if (counterData)
		{
			mTraversalTask->AtomicCounterCache = *counterData;
			counterData->writeCount = 0;
			counterData->temp = 0;
		}
		mTraversalTask->mAtomicCounterBuffer->Unmap();
#else
		unsigned int counterData[2];
		counterData[0] = 0;
		counterData[1] = 0;
		mTraversalTask->mAtomicCounterBuffer->Bind(0);
		//mTraversalTask->mAtomicCounterBuffer->Clear(BIF_R32UI, BF_R32UI, BCT_Unsigned_Int, counterData);
#endif

		// Execute traversal task
		mTraversalTask->mACMSharedCommandBuffer->Bind(0);
		mTraversalTask->mACMBuffer->Bind(1);
		//*
		mTraversalTask->DispatchCompute(0,
			4 * mTraversalTask->AtomicCounterCache.writeCount,
			1,
			1);
		//*/

		//mTraversalTask->mACMSharedCommandBuffer->Bind();
		//auto bufferData = (ACMSharedCommandBuffer*)mTraversalTask->mACMSharedCommandBuffer->Map(BA_Read_Write);
		//mTraversalTask->mACMSharedCommandBuffer->Unmap();



		mTraversalTask->DispatchCompute(1, 1, 1, 1);
#ifdef DEBUG_CAUSTCIS
		auto bufferData = (unsigned int*)mTraversalTask->mACMBuffer->Map(BA_Read_Write);
		mTraversalTask->mACMBuffer->Unmap();
#endif
		//mTraversalTask->mACMSharedCommandBuffer->Bind();
		//bufferData = (ACMSharedCommandBuffer*)mTraversalTask->mACMSharedCommandBuffer->Map(BA_Read_Write);
		//mTraversalTask->mACMSharedCommandBuffer->Unmap();
		// Generate statistics
		//*
		mTraversalTask->mAtomicCounterBuffer->Bind();
		auto counterData1 = (ACMAtomicCounter*)mTraversalTask->mAtomicCounterBuffer->Map(BA_Read_Write);
		assert(counterData1);
		if (counterData1)
		{
			mTraversalTask->AtomicCounterCache = *counterData1;
		}
		mTraversalTask->mAtomicCounterBuffer->Unmap();
		//*/

		/* Increment uniform data for next lod
		auto uniformData = (ACMSharedCommandBuffer*)mTraversalTask->mACMSharedCommandBuffer->Map(BA_Read_Write);
		assert(uniformData);
		if (uniformData)
		{
			mTraversalTask->UniformCache = *uniformData;
			uniformData->readOffset += mTraversalTask->UniformCache.readCount;
			uniformData->readCount = 4 * mTraversalTask->AtomicCounterCache.writeCount;
			uniformData->writeOffset += 4 * mTraversalTask->AtomicCounterCache.writeCount;
		}
		mTraversalTask->mACMSharedCommandBuffer->Unmap();
		//*/
	}

	SubRenderer::PostRender(0, 0);
}
//----------------------------------------------------------------------------