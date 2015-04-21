#include "CausticMapRenderer.h"
#include "DeferredRefractionRenderer.h"

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
	TraversalLevel = 1;
}
//----------------------------------------------------------------------------
CausticMapRenderer::~CausticMapRenderer()
{
	mPSB = 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
AdaptiveCausticsTaskInfo::AdaptiveCausticsTaskInfo()
{
	RefractionIndex = 1.1f;
}
//----------------------------------------------------------------------------
AdaptiveCausticsTaskInfo::~AdaptiveCausticsTaskInfo()
{}
//----------------------------------------------------------------------------
void AdaptiveCausticsTaskInfo::OnGetShaderConstants()
{
	auto program = this->GetPass(0)->GetShaderProgram();
	program->GetUniformLocation(&ImageUnit0Loc, "data");
	program->GetUniformLocation(&ImageUnit1Loc, "refractorNormal");

	if (this->GetPassCount() >= 2)
	{
		program = this->GetPass(2)->GetShaderProgram();
		program->GetUniformLocation(&splatResolutionModifierLoc, "splatResolutionModifier");
		program->GetUniformLocation(&renderBufResLoc, "renderBufRes");
		program->GetUniformLocation(&lightProjLoc, "LightProj");

		program->GetUniformLocation(&TanLightFovy2Loc, "TanLightFovy2");
		program->GetUniformLocation(&NearFarInfoLoc, "NearFarInfo");
		program->GetUniformLocation(&RefractionIndexInfoLoc, "RefractionIndexInfo");
		program->GetUniformLocation(&RefractorNormTexLoc, "RefractorNorm");
		program->GetUniformLocation(&RefractorDepthTexLoc, "RefractorDepth");
		program->GetUniformLocation(&ReceiverDepthTexLoc, "ReceiverDepth");
	}
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
		mComputeTempTexture->BindToImageUnit(0, BufferAccess::BA_Write_Only);
		//mNormalTextures->BindToImageUnit(1, BufferAccess::BA_Read_Only, true);
		mRefractorNormalTextures->BindToSampler(1, &sampler);
	}
	if (pass == 2)
	{
		int maxPhotonRes = (int)(pow(2.0, (float)GMaxTraversalLevel + 5) + 0.1);
		splatResolutionModifierLoc.SetValue(1024.0f / maxPhotonRes);
		renderBufResLoc.SetValue(1024.0f);
		lightProjLoc.SetValue(mCamera->GetProjectionTransform());
		TanLightFovy2Loc.SetValue(glm::pi<float>() * mCamera->GetFoV() / 360.0f);

		float nearFarPlane[2];
		mCamera->GetNearFarPlane(nearFarPlane);
		NearFarInfoLoc.SetValue(vec4(
			nearFarPlane[0] * nearFarPlane[1],
			nearFarPlane[1] - nearFarPlane[0],
			nearFarPlane[1] + nearFarPlane[0], nearFarPlane[1]));

		RefractionIndexInfoLoc.SetValue(vec4(
			1.f / RefractionIndex,
			1.f / (RefractionIndex * RefractionIndex),
			RefractionIndex,
			RefractionIndex * RefractionIndex));

		RefractorNormTexLoc.SetValue(0);
		RefractorDepthTexLoc.SetValue(1);
		ReceiverDepthTexLoc.SetValue(2);
		sampler.MinFilter = FT_Linear_Linear;
		mRefractorNormalTextures->BindToSampler(0, &sampler);
		mRefractorDepthTextures->BindToSampler(1, &sampler);
		sampler.MinFilter = FT_Linear;
		mReceiverDepthTexture->BindToSampler(2, &sampler);

		//ImageUnit0Loc.SetValue(0);
		//ImageUnit1Loc.SetValue(1);
		//mCompTexture->BindToImageUnit(0, BufferAccess::BA_Write_Only);
		//mNormalTextures->BindToImageUnit(1, BufferAccess::BA_Read_Only, true);
		//mNormalTextures->BindToSampler(1, &sampler);
	}

	glMemoryBarrier(GL_ATOMIC_COUNTER_BARRIER_BIT);
}
//----------------------------------------------------------------------------
void AdaptiveCausticsTaskInfo::OnPostDispatch(unsigned int pass)
{
	glMemoryBarrier(GL_COMMAND_BARRIER_BIT);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
void RTGI::CausticMapRenderer::Initialize(GPUDevice* device,
	CausticsMapDesc* desc,
	ReceiverResourceRenderer* receiverResourceRenderer,
	RefractorResourceRenderer* refractorResourceRenderer,
	Camera* light)
{

	// Cache temp buffer and textures needed for visualization.
	mReceiverPositionTexture =
		(Texture2D*)receiverResourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_ReceiverPosition_Name);
	mReceiverDepthTexture =
		(Texture2D*)receiverResourceRenderer->GetDepthTexture();
	mRefractorNormalTextures =
		(Texture2DArray*)refractorResourceRenderer->GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_RefractorFrontAndBackNormal_Name);
	mRefractorDepthTextures =
		(Texture2DArray*)refractorResourceRenderer->GetDepthTexture();

	assert(desc);

	AddFrameBufferTarget(RTGI_CausticsBuffer_CausticMap_Name,
		desc->Width, desc->Height, 0, TT_Texture2D,
		desc->CausticsMapFormat, desc->CausticsMapMipmap);
	CreateFrameBuffer(desc->Width, desc->Height, 0, TT_Texture2D);

	// Create gather voxel fragment list info task.
	ShaderProgramInfo PI_AdaptiveCausticsTraversal;
	PI_AdaptiveCausticsTraversal
		<< "AdaptiveCaustics/CausticsTraversal/AdaptiveCausticsTraversal.comp";

	ShaderProgramInfo PI_AdaptiveCausticsPostTraversalProcess;
	PI_AdaptiveCausticsPostTraversalProcess
		<< "AdaptiveCaustics/CausticsTraversal/AdaptiveCausticsPostTraversalProcess.comp";

	ShaderProgramInfo PI_AdaptiveCausticsDebugDraw;
	PI_AdaptiveCausticsDebugDraw <<
		"AdaptiveCaustics/CausticsTraversal/AdaptiveCausticsDrawDebug.comp";

	ShaderProgramInfo PI_AdaptiveCausticsSplat;
	PI_AdaptiveCausticsSplat
		// 		<< "AdaptiveCaustics/CausticsSplat/CausticsSplat.vert"
		// 		<< "AdaptiveCaustics/CausticsSplat/CausticsSplat.frag";
		<< "AdaptiveCaustics/CausticsSplat/CausticsSplatTest.vert"
		<< "AdaptiveCaustics/CausticsSplat/CausticsSplatTest.frag";

// 	ShaderProgramInfo PI_CopyDepth;
// 	PI_CopyDepth
// 		// 		<< "AdaptiveCaustics/CausticsSplat/CausticsSplat.vert"
// 		// 		<< "AdaptiveCaustics/CausticsSplat/CausticsSplat.frag";
// 		<< "AdaptiveCaustics/CopyTexture/CopyTexture.vert"
// 		<< "AdaptiveCaustics/CopyTexture/CopyTextureDepth.frag";

	ComputePass* PassInfo_AdaptiveCausticsTraversal = new ComputePass(
		PI_AdaptiveCausticsTraversal);
	ComputePass* PassInfo_AdaptivePostTraversalProcess = new ComputePass(
		PI_AdaptiveCausticsPostTraversalProcess);
	ComputePass* PassInfo_AdaptiveCausticsDebugDraw = new ComputePass(
		PI_AdaptiveCausticsDebugDraw);
	ComputePass* PassInfo_AdaptiveSplat = new ComputePass(
		PI_AdaptiveCausticsSplat);

	mCausticsTask = new AdaptiveCausticsTaskInfo();
	mCausticsTask->AddPass(PassInfo_AdaptiveCausticsTraversal);
	mCausticsTask->AddPass(PassInfo_AdaptivePostTraversalProcess);
	mCausticsTask->AddPass(PassInfo_AdaptiveSplat);
	mCausticsTask->CreateDeviceResource(mDevice);


	mDebugDrawTask = new AdaptiveCausticsTaskInfo();
	mDebugDrawTask->AddPass(PassInfo_AdaptiveCausticsDebugDraw);
	mDebugDrawTask->CreateDeviceResource(mDevice);

// 	MaterialTemplate* mtCopyTexture =
// 		new MaterialTemplate(
// 		new Technique(new Pass(PI_CopyDepth)));
// 
// 	mCopyTextureScreenQuad = new CopyTextureScreenQuad(
// 		new Material(mtCopyTexture));
// 	mCopyTextureScreenQuad->LoadFromPLYFile("screenquad.ply");
// 	mCopyTextureScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
// 	mCopyTextureScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
// 	mCopyTextureScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
// 	mCopyTextureScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
// 	mCopyTextureScreenQuad->CreateDeviceResource(device);

	// Now we need more generic, large traversal buffers to ping-pong between
	//     for other rendering modes.  Here I allocate an array of these.  Many
	//     render modes need only 1 or 2, those some of the complex ones need
	//     4 or 5.  They probably need not all be the same size, but if I use
	//     different sizes, I have to remember which is which!  (And then they're
	//     not generic!).

	mCompTexture = new Texture2D();
	mCompTexture->CreateRenderTarget(mDevice, 768, 768, BF_RGBA16F);

	mCausticsTexture = (Texture2D*)GetFrameBufferTextureByName(
		RTGI_CausticsBuffer_CausticMap_Name);

	// Clear without drawing
	mFBOCaustics = new FrameBuffer(mDevice);
	mFBOCaustics->SetRenderTargets(1, (Texture**)&mCausticsTexture);

	// Clear without drawing
	mFBOComputeTemp = new FrameBuffer(mDevice);
	mFBOComputeTemp->SetRenderTargets(1, (Texture**)&mCompTexture);

	mCausticsTask->mAtomicCounterBuffer = new AtomicCounterBuffer();
#ifdef DEBUG_CAUSTCIS
	mCausticsTask->mAtomicCounterBuffer->ReserveMutableDeviceResource(mDevice,
		sizeof(ACMAtomicCounter), BU_Dynamic_Copy);
#else
	mCausticsTask->mAtomicCounterBuffer->ReserveMutableDeviceResource(mDevice,
		sizeof(ACMAtomicCounter), BU_Dynamic_Copy);
#endif

	// Initialize atomic counter
	mCausticsTask->AtomicCounterCache.writeCount = START_KERNEL_SIZE / 4;
	mCausticsTask->AtomicCounterCache.temp = 0;
	mCausticsTask->mAtomicCounterBuffer->UpdateSubData(0, 0, sizeof(ACMAtomicCounter),
		(void*)&mCausticsTask->AtomicCounterCache);
	//mTraversalTask->mAtomicCounterBuffer->Clear(BIF_R32UI, BF_R32UI, BCT_Unsigned_Int,
	//	counterData0);
	// Storage buffer
	mCausticsTask->mACMBuffer = new StructuredBuffer();
	auto bufferSize = sizeof(ACMBuffer)+KERNEL_SIZE(11) * sizeof(vec4);
	mCausticsTask->mACMBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Stream_Copy);
	InitializeMinCausticHierarchy(mDevice, mCausticsTask, START_KERNEL_WIDTH);

	// Shared Command buffer
	mCausticsTask->mACMSharedCommandBuffer = new StructuredBuffer();
	auto bufferSize1 = sizeof(ACMSharedCommandBuffer);
	mCausticsTask->mACMSharedCommandBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Copy);

	// Shared command buffer for debug draw
	mDebugDrawTask->mACMSharedCommandBuffer = mCausticsTask->mACMSharedCommandBuffer;

	// Setup input and output
	mCausticsTask->mComputeTempTexture = mCompTexture;
	mCausticsTask->mRefractorNormalTextures = mRefractorNormalTextures;
	mCausticsTask->mRefractorDepthTextures = mRefractorDepthTextures;
	mCausticsTask->mReceiverDepthTexture = mReceiverDepthTexture;
	mCausticsTask->mCamera = light;

#ifdef _DEBUG
	GLenum res = glGetError();
	assert(res == GL_NO_ERROR);
#endif
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

	mCausticsTask->mACMBuffer->Bind(0);
	mCausticsTask->mACMBuffer->UpdateSubData(0, sizeof(ACMBuffer), bufSize, causticStartPoints);

	free(causticStartPoints);

	//pVB->LoadFromSystemMemory(pDevice, bufSize, causticStartPoints, RTGI::BufferUsage::BU_Static_Draw);

	//mPoints->LoadFromSystemMemory(bufSize/4, causticStartPoints, 4);

	//mPoints->CreateDeviceResource(mDevice);

}
//----------------------------------------------------------------------------
void CausticMapRenderer::CreateCausticsResource(CausticsMapDesc* desc)
{

}

//#define DEBUG_CAUSTCIS
//----------------------------------------------------------------------------
void CausticMapRenderer::Render(int technique, int pass, Camera* camera)
{
	SubRenderer::PreRender(0, 0);
	DoTraversal();
	DoSplat();
	SubRenderer::PostRender(0, 0);

}
//----------------------------------------------------------------------------
void CausticMapRenderer::DoTraversal()
{
	mCausticsTask->TraversalLevel = TraversalLevel;

	mFBOComputeTemp->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mFBOComputeTemp->Disable();

	if (DrawDebugMipmap)
	{
		mDebugDrawTask->mComputeTempTexture = mCompTexture;
		mDebugDrawTask->mRefractorNormalTextures = mRefractorNormalTextures;

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
		mCausticsTask->mACMSharedCommandBuffer->Bind(0);
		mCausticsTask->mACMBuffer->Bind(1);
		mDebugDrawTask->DispatchCompute(0, mCompTexture->Width, mCompTexture->Height, 1);
	}

	// Reset atomic counters.
#ifdef DEBUG_CAUSTCIS
	auto counterData0 =
		(ACMAtomicCounter*)mCausticsTask->mAtomicCounterBuffer->Map(BA_Read_Write);
	assert(counterData0);
	if (counterData0)
	{
		mCausticsTask->AtomicCounterCache = *counterData0;
		counterData0->writeCount = START_KERNEL_SIZE / 4;
		counterData0->temp = 0;
	}
	mCausticsTask->mAtomicCounterBuffer->Unmap();
#else
	static unsigned int counterData0[2];
	counterData0[0] = 0;
	counterData0[1] = 0;
	mCausticsTask->mAtomicCounterBuffer->Clear(BIF_R32UI, BF_R32UI, BCT_Unsigned_Int,
		counterData0);
#endif

	// Initialize uniform buffer
#ifdef DEBUG_CAUSTCIS
	auto uniformData = (ACMUniformBuffer*)mCausticsTask->mACMUniformBuffer->Map(BA_Read_Write);
#else
	mCausticsTask->mACMSharedCommandBuffer->Bind();
	auto uniformData = (ACMSharedCommandBuffer*)mCausticsTask->mACMSharedCommandBuffer->Map(BA_Write_Only);
#endif
	assert(uniformData);
	if (uniformData)
	{
		uniformData->mipmapLevel = (int)(log2(float(mRefractorNormalTextures->Width)) + 0.01) - (uint)START_LOD;
		uniformData->readOffset = 0;
		uniformData->readCount = START_KERNEL_SIZE;
		uniformData->writeOffset = START_KERNEL_SIZE;
		uniformData->width = mCompTexture->Width;
		uniformData->height = mCompTexture->Height;
		uniformData->deltaX = 0.5f / (float)START_KERNEL_WIDTH;
		uniformData->deltaY = 0.5f / (float)START_KERNEL_WIDTH;
	}
	mCausticsTask->mACMSharedCommandBuffer->Unmap();

	// Initialize uniform buffer cache
	mCausticsTask->UniformCache.mipmapLevel = 0;
	mCausticsTask->UniformCache.readOffset = 0;
	mCausticsTask->UniformCache.readCount = START_KERNEL_SIZE;
	mCausticsTask->UniformCache.writeOffset = START_KERNEL_SIZE / 4; // Every vertex plits into 4
	mCausticsTask->UniformCache.width = mCompTexture->Width;
	mCausticsTask->UniformCache.height = mCompTexture->Height;
	mCausticsTask->UniformCache.deltaX = 0.5f / (float)START_KERNEL_WIDTH;
	mCausticsTask->UniformCache.deltaY = 0.5f / (float)START_KERNEL_WIDTH;

	// Initialize atomic counter
	mCausticsTask->AtomicCounterCache.writeCount = START_KERNEL_SIZE / 4;
	mCausticsTask->AtomicCounterCache.temp = 0;

	/*
	int result[6];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &result[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &result[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &result[2]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &result[3]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, 0, &result[4]);
	*/
	uint currentLOD = (uint)START_LOD;  // The starting traversal level (2^6 = 64x64 photons)
	int numLevels = (int)(log2(float(mRefractorNormalTextures->Width)) + 0.01) - currentLOD;

	int arg = min(TraversalLevel, numLevels + 6);

	for (int i = 0; i < arg; ++i, ++currentLOD)
	{
		// We have a specific resolution level to start at (64x64, 2^6 x 2^6, or "level" 6).  
		//   Unfortunately, mipmaps count "0" as the highest resolution, and we count "0" 
		//   as a 1 x 1 image, so we need this computation to get the correct mipmap level 
		//   for our shader.  This is then decremented (because we increase resolution) 
		//   each step as we traverse.
		int glMipmapLevel = (int)(log2(float(mRefractorNormalTextures->Width)) + 0.01) - currentLOD;

		// What resolution is the start of our traversal at?  We'll have 
		//   startResolution*startResolution photon clusters to process during
		//   the first traversal step.
		int currentResolution = KERNEL_WIDTH(currentLOD);

		// Reset atomic counters.
#ifdef DEBUG_CAUSTCIS
		mCausticsTask->mAtomicCounterBuffer->Bind(0);
		auto counterData = (ACMAtomicCounter*)mCausticsTask->mAtomicCounterBuffer->Map(BA_Read_Write);
		assert(counterData);
		if (counterData)
		{
			mCausticsTask->AtomicCounterCache = *counterData;
			counterData->writeCount = 0;
			counterData->temp = 0;
		}
		mCausticsTask->mAtomicCounterBuffer->Unmap();
#else
		unsigned int counterData[2];
		counterData[0] = 0;
		counterData[1] = 0;
		mCausticsTask->mAtomicCounterBuffer->Bind(0);
		mCausticsTask->mAtomicCounterBuffer->Clear(BIF_R32UI, BF_R32UI, BCT_Unsigned_Int, counterData);
#endif

		// Execute traversal task
		mCausticsTask->mACMSharedCommandBuffer->Bind(0);
		mCausticsTask->mACMBuffer->Bind(1);
		//*
		auto res = (int)(glm::sqrt((float)mCausticsTask->AtomicCounterCache.writeCount) + 1.0f);
		mCausticsTask->DispatchCompute(0,
			2 * res,
			2 * res,
			1);
		//*/

		//mTraversalTask->mACMSharedCommandBuffer->Bind();
		//auto bufferData = (ACMSharedCommandBuffer*)mTraversalTask->mACMSharedCommandBuffer->Map(BA_Read_Write);
		//mTraversalTask->mACMSharedCommandBuffer->Unmap();



		mCausticsTask->DispatchCompute(1, 1, 1, 1);
#ifdef DEBUG_CAUSTCIS
		auto bufferData = (unsigned int*)mCausticsTask->mACMBuffer->Map(BA_Read_Write);
		mCausticsTask->mACMBuffer->Unmap();
#endif
		//mTraversalTask->mACMSharedCommandBuffer->Bind();
		//bufferData = (ACMSharedCommandBuffer*)mTraversalTask->mACMSharedCommandBuffer->Map(BA_Read_Write);
		//mTraversalTask->mACMSharedCommandBuffer->Unmap();
		// Generate statistics
		//*
		mCausticsTask->mAtomicCounterBuffer->Bind(0);
		auto counterData1 = (ACMAtomicCounter*)mCausticsTask->mAtomicCounterBuffer->Map(BA_Read_Write);
		assert(counterData1);
		if (counterData1)
		{
			mCausticsTask->AtomicCounterCache = *counterData1;
		}
		mCausticsTask->mAtomicCounterBuffer->Unmap();
		//*/
	}

}
//----------------------------------------------------------------------------
void CausticMapRenderer::DoSplat()
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	mFBOCaustics->Enable();
	mCausticsTask->mAtomicCounterBuffer->Bind(0);
	mCausticsTask->mACMSharedCommandBuffer->Bind(0);
	mCausticsTask->mACMBuffer->Bind(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	mCausticsTask->DispatchVertex(2, 4 * mCausticsTask->AtomicCounterCache.writeCount);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_BLEND);
	mFBOCaustics->Disable();
	glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
	glEnable(GL_DEPTH_TEST);

	//mCausticsTexture->GenerateMipmap();
}