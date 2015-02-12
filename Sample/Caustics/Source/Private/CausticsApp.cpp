#include "CausticsApp.h"
#include <glfw3.h>
using namespace RTGI;

//----------------------------------------------------------------------------
CausticsApp::CausticsApp(int width, int height)
{
	mShowMode = SM_Position;
	Width = width;
	Height = height;
	Title = "Caustics demo";
}
//----------------------------------------------------------------------------
CausticsApp::~CausticsApp()
{
}
//----------------------------------------------------------------------------
void CausticsApp::Initialize(GPUDevice* device)
{
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create camera and light
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width / (float)Height, 0.1f, 50.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	
	auto mLightCamera = new Camera;
	mLightCamera->SetPerspectiveFrustum(75.0f, (float)Width / (float)Height, 1.0f, 50.0f);
	mLightCamera->SetLookAt(vec3(-0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
	auto test = mLightCamera->GetViewTransform();
	auto redUV = test * vec4(1, -1, -1, 1);
	redUV = redUV / length(redUV);

	mLight = new Light;
	mLight->SetProjector(mLightCamera);
	mLight->Color = vec3(0.9f, 0.9f, 0.7f);

	// G-buffer for receiver
	mReceiverPositionTexture = new Texture2D();
	mReceiverNormalTexture = new Texture2D();
	mReceiverColorTexture = new Texture2D();
	mReceiverDepthTexture = new Texture2D();
	mReceiverPositionTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mReceiverNormalTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mReceiverColorTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mReceiverDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mReceiverGBuffer = new FrameBuffer(mDevice);
	Texture* receiverColorTextures[3] = { mReceiverPositionTexture, mReceiverNormalTexture, mReceiverColorTexture };
	mReceiverGBuffer->SetRenderTargets(3, receiverColorTextures, mReceiverDepthTexture);

	// G-buffer for refractive obj
	mRefracPositionTexture = new Texture2D();
	mRefracNormalTexture = new Texture2D();
	mRefracColorTexture = new Texture2D();
	mRefracDepthTexture = new Texture2D();
	mRefracPositionTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mRefracNormalTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mRefracColorTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mRefracDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mRefracGBuffer = new FrameBuffer(mDevice);
	Texture* RefracColorTextures[3] = { mRefracPositionTexture, mRefracNormalTexture, mRefracColorTexture };
	mRefracGBuffer->SetRenderTargets(3, RefracColorTextures, mRefracDepthTexture);

	// G-buffer for receiver object from light pov
	mLightReceiverPositionTexture = new Texture2D();
	mLightReceiverNormalTexture = new Texture2D();
	mLightReceiverColorTexture = new Texture2D();
	mLightReceiverDepthTexture = new Texture2D();
	mLightReceiverPositionTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mLightReceiverNormalTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mLightReceiverColorTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mLightReceiverDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mRecvGBufferLight = new FrameBuffer(mDevice);
	Texture* LightRecvColorTextures[3] = { mLightReceiverPositionTexture, mLightReceiverNormalTexture, mLightReceiverColorTexture };
	mRecvGBufferLight->SetRenderTargets(3, LightRecvColorTextures, mLightReceiverDepthTexture);

	// G-buffer for refractive object from light pov
	mLightRefracPositionTexture = new Texture2D();
	mLightRefracNormalTexture = new Texture2D();
	mLightRefracColorTexture = new Texture2D();
	mLightRefracDepthTexture = new Texture2D();
	mLightRefracPositionTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mLightRefracNormalTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mLightRefracColorTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mLightRefracDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mRefracGBufferLight = new FrameBuffer(mDevice);
	Texture* refracColorTextures[3] = { mLightRefracPositionTexture, mLightRefracNormalTexture, mLightRefracColorTexture };
	mRefracGBufferLight->SetRenderTargets(3, refracColorTextures, mLightRefracDepthTexture);

	// G-buffer for intersection points
	mIntersectionPositionTexture = new Texture2D();
	mIntersectionDepthTexture = new Texture2D();
	mIntersectionPositionTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mIntersectionDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mIntersectionGBuffer = new FrameBuffer(mDevice);
	Texture* IntersectionColorTextures[1] = { mIntersectionPositionTexture };
	mIntersectionGBuffer->SetRenderTargets(1, IntersectionColorTextures, mIntersectionDepthTexture);


	// G-buffer for caustics map
	mCausticsMapTexture = new Texture2D();
	mCausticsMapDepthTexture = new Texture2D();
	mCausticsMapTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mCausticsMapDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mCausticsMapGBuffer = new FrameBuffer(mDevice);
	Texture* CausticsMapColorTextures[1] = { mCausticsMapTexture };
	mCausticsMapGBuffer->SetRenderTargets(1, CausticsMapColorTextures, mCausticsMapDepthTexture);

	// G-buffer for blurred caustics map
	mBlurredCausticsMapTexture = new Texture2D();
	mBlurredCausticsMapDepthTexture = new Texture2D();
	mBlurredCausticsMapTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mBlurredCausticsMapDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mBlurredCausticsMapGBuffer = new FrameBuffer(mDevice);
	Texture* BlurredCausticsMapColorTextures[1] = { mBlurredCausticsMapTexture };
	mBlurredCausticsMapGBuffer->SetRenderTargets(1, BlurredCausticsMapColorTextures, mBlurredCausticsMapDepthTexture);

	// Create material templates.
	mCausticsDebugBuffer = new StructuredBuffer();
	auto bufferSize = sizeof(CausticsDebugBuffer) + this->Width * this->Height * sizeof(vec3);
	mCausticsDebugBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Copy);

	// G-buffer for final image
	mReceiverTexture = new Texture2D();
	mRefracTexture = new Texture2D();
	mDepthTexture = new Texture2D();
	mReceiverTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mRefracTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mGBufferFinal = new FrameBuffer(mDevice);
	Texture* finalColorTextures[2] = { mReceiverTexture, mRefracTexture };
	mGBufferFinal->SetRenderTargets(2, finalColorTextures, mDepthTexture);




	// Scene elements
	ShaderProgramInfo gbufferProgramInfo;
	gbufferProgramInfo << "Caustics/GBuffer.vert" << "Caustics/GBuffer.frag";
	auto mtGBuffer = new MaterialTemplate(new Technique(new Pass(gbufferProgramInfo)));

	mGround = new CausticsTriMesh(new Material(mtGBuffer), mMainCamera);
	mGround->LoadFromFile("ground.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource(mDevice);
	mGround->SetWorldTranslation(vec3(0.0f, -6.0f, 0.0f));
	mGround->MaterialColor = vec3(1.0f, 1.0f, 1.0f);

	mMesh = new CausticsTriMesh(new Material(mtGBuffer), mMainCamera);
	mMesh->LoadFromFile("dragon_s.ply");
	mMesh->GenerateNormals();
	mMesh->CreateDeviceResource(mDevice);
	mMesh->SetWorldTransform(rotate(mat4(), radians(30.0f), vec3(0, 1, 0)));
	mMesh->SetWorldTranslation(vec3(0.0f, -0.8f, 0.0f));
	mMesh->SetWorldScale(vec3(3.0f));
	mMesh->MaterialColor = vec3(1.0f, 1.0f, 1.0f);


	auto mCubeMap = new TextureCube();
	mCubeMap->LoadFromFile(mDevice,
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp");

	ShaderProgramInfo gbufferCubeProgramInfo;
	gbufferCubeProgramInfo << "Caustics/GBufferCube.vert" << "Caustics/GBufferCube.frag";
	auto mtGBufferCube = new MaterialTemplate(new Technique(new Pass(gbufferCubeProgramInfo)));

	mPool = new CausticsCube(new Material(mtGBufferCube), mMainCamera);
	mPool->LoadFromFile("cube.ply");
	mPool->GenerateNormals();
	mPool->CreateDeviceResource(mDevice);
	mPool->SetWorldTranslation(vec3(0.0f, 0.0f, 0.0f));
	mPool->SetWorldScale(vec3(1, -1, 1));
	mPool->MaterialColor = vec3(1, 1, 1);
	mPool->CubeTexture = mCubeMap;
	mPool->VertexSplattingTexture = mLightRefracPositionTexture;
	mPool->Light = mLight;


	// caustics map processor
	vec2 tcoord00(0.0f, 0.0f);
	vec2 tcoord01(0.0f, 1.0f);
	vec2 tcoord10(1.0f, 0.0f);
	vec2 tcoord11(1.0f, 1.0f);
	ShaderProgramInfo SICausticsMapIntersect;
	SICausticsMapIntersect << "Caustics/Intersection.vert"  << "Caustics/Intersection.frag";
	ShaderProgramInfo SIGaussianBlurH;
	SIGaussianBlurH << "Caustics/GaussianBlurH.vert" << "Caustics/GaussianBlurH.frag";
	ShaderProgramInfo SIGaussianBlurV;
	SIGaussianBlurV << "Caustics/GaussianBlurV.vert" << "Caustics/GaussianBlurV.frag";
	ShaderProgramInfo SICausticsReceiverFinal;
	SICausticsReceiverFinal << "Caustics/CausticsReceiverFinal.vert" << "Caustics/CausticsReceiverFinal.frag";
	ShaderProgramInfo SICausticsRefracFinal;
	SICausticsRefracFinal << "Caustics/CausticsRefracFinal.vert" << "Caustics/CausticsRefracFinal.frag";
	auto mtCausticsMap = new MaterialTemplate(new Technique({
		new Pass(SICausticsMapIntersect),
		new Pass(SIGaussianBlurH),
		new Pass(SIGaussianBlurV),
		new Pass(SICausticsReceiverFinal),
		new Pass(SICausticsRefracFinal) }));


	mCausticsScreenQuad = new CausticsScreenQuad(new Material(mtCausticsMap), mMainCamera);
	mCausticsScreenQuad->LoadFromFile("screenquad.ply");
	mCausticsScreenQuad->SetTCoord(0, tcoord00);
	mCausticsScreenQuad->SetTCoord(1, tcoord10);
	mCausticsScreenQuad->SetTCoord(2, tcoord11);
	mCausticsScreenQuad->SetTCoord(3, tcoord01);
	mCausticsScreenQuad->CreateDeviceResource(mDevice);
	// Pass 0
	mCausticsScreenQuad->ReceiverPositionLightTexture = mLightReceiverPositionTexture;
	mCausticsScreenQuad->ReceiverNormalLightTexture = mLightReceiverNormalTexture;
	mCausticsScreenQuad->ReceiverReflectanceLightTexture = mLightReceiverColorTexture;

	mCausticsScreenQuad->RefracterPositionLightTexture = mLightRefracPositionTexture;
	mCausticsScreenQuad->RefracterNormalLightTexture = mLightRefracNormalTexture;
	mCausticsScreenQuad->ReceiverReflectanceLightTexture = mLightRefracColorTexture;
	mCausticsScreenQuad->RefractionIndex = 0.95f;
	mCausticsScreenQuad->CubeTexture = mCubeMap;
	mCausticsScreenQuad->Light = mLight;

	// Pass 1, 2
	mCausticsScreenQuad->CausticsMapTexture = mCausticsMapTexture;

	// Final Render receiver
	mCausticsScreenQuad->BlurredCausticsMapTexture = mBlurredCausticsMapTexture;
	mCausticsScreenQuad->ReceiverPositionTexture = mReceiverPositionTexture;
	mCausticsScreenQuad->ReceiverNormalTexture = mReceiverNormalTexture;
	mCausticsScreenQuad->ReceiverColorTexture = mReceiverColorTexture;
	mCausticsScreenQuad->ShadowmapTexture = mLightRefracDepthTexture;

	// Final render refracter
	mCausticsScreenQuad->RefracPositionTexture = mRefracPositionTexture;
	mCausticsScreenQuad->RefracNormalTexture = mRefracNormalTexture;
	mCausticsScreenQuad->RefracColorTexture = mRefracColorTexture;
	mCausticsScreenQuad->ShadowmapTexture2 = mLightRefracDepthTexture;

	ShaderProgramInfo SICausticsMap;
	SICausticsMap << "Caustics/CausticsMapPointGathering.vert" << "Caustics/CausticsMapPointGathering.frag";
	auto mtVertexGrid = new MaterialTemplate(new Technique(new Pass(SICausticsMap)));
	mVertexGrid = new VertexGrid(this->Width, this->Height, new Material(mtVertexGrid));

	mVertexGrid->CreateDeviceResource(mDevice);
	// Pass 0
	mVertexGrid->CausticsMapsResolution = 0;
	mVertexGrid->CausticsMapTexture = mCausticsMapTexture;
	mVertexGrid->IntersectionPositionTexture = mIntersectionPositionTexture;
	mVertexGrid->Light = mLight;

}

//----------------------------------------------------------------------------
void CausticsApp::DrawReceiverLightPoV()
{
	mGround->SetCamera(mLight->GetProjector());
	mPool->SetCamera(mLight->GetProjector());
	//mGround->Render(0, 0);
	mPool->Render(0, 0);
	mGround->SetCamera(mMainCamera);
	mPool->SetCamera(mMainCamera);
}

void CausticsApp::DrawRefracterLightPoV()
{
	mMesh->SetCamera(mLight->GetProjector());
	mMesh->Render(0, 0);
	mMesh->SetCamera(mMainCamera);
}

void CausticsApp::DrawReceiverCameraPoV()
{
	mPool->Render(0, 0);
}

void CausticsApp::DrawRefracCameraPoV()
{
	mMesh->Render(0, 0);
}
//----------------------------------------------------------------------------
void CausticsApp::FrameFunc()
{
	// Draw Receiver to G-buffer from Light's PoV.
	mRecvGBufferLight->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CCW);
	DrawReceiverLightPoV();
	mRecvGBufferLight->Disable();

	// Draw Refractive object to G-buffer from Light's PoV.
	mRefracGBufferLight->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CCW);
	DrawRefracterLightPoV();
	mRefracGBufferLight->Disable();
	
	// Draw Receiver Camera pov
	mReceiverGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CCW);
	DrawReceiverCameraPoV();
	mReceiverGBuffer->Disable();

	// Draw Refractive obj Camera pov
	mRefracGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CCW);
	DrawRefracCameraPoV();
	mRefracGBuffer->Disable();

	glFrontFace(GL_CCW);

	// Draw intersection points

	GLuint occlusionQuery = 0;
	unsigned int numPixels = 0;
	glGenQueries(1, &occlusionQuery);

	glBeginQuery(GL_SAMPLES_PASSED, occlusionQuery);
	mIntersectionGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mCausticsScreenQuad->Render(0, 0);
	mIntersectionGBuffer->Disable();
	glEndQuery(GL_SAMPLES_PASSED);
	glGetQueryObjectuiv(occlusionQuery, GL_QUERY_RESULT, &numPixels);
	mVertexGrid->CausticsMapsResolution = numPixels;

	//*
	// Draw Caustics Map
	glBeginQuery(GL_PRIMITIVES_GENERATED, occlusionQuery);
	mCausticsMapGBuffer->Enable();
	mCausticsDebugBuffer->Bind(3);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	mVertexGrid->Render(0, 0);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	mCausticsDebugBuffer->Bind();
	mCausticsMapGBuffer->Disable();
	glEndQuery(GL_PRIMITIVES_GENERATED);
	glGetQueryObjectuiv(occlusionQuery, GL_QUERY_RESULT, &numPixels);
	//printf("GL_PRIMITIVES_GENERATED: %i\n", numPixels);
	//*/

	/*
	void* bufferData = mCausticsDebugBuffer->Map(BA_Read_Only);
	auto dataPtr = (CausticsDebugBuffer*)bufferData;
	auto dataPtr2 = (vec3*)(dataPtr + 1);
	mCausticsDebugBuffer->Unmap();
	*/

	//*
	// Blur caustics map
	mBlurredCausticsMapGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	mCausticsScreenQuad->Render(0, 1);
	mCausticsScreenQuad->Render(0, 2);
	glEnable(GL_DEPTH_TEST);
	mBlurredCausticsMapGBuffer->Disable();
	//*/

	// Draw final image
	//mGBufferFinal->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mCausticsScreenQuad->Render(0, 3);
	mCausticsScreenQuad->Render(0, 4);
	//mGBufferFinal->Disable();

	/* Debug g buffer
	// Draw Receiver Camera pov
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glFrontFace(GL_CCW);
	DrawReceiverCameraPoV();
	glFrontFace(GL_CW);
	DrawRefracCameraPoV();
	//*/

	switch (mShowMode)
	{
		break;
	default:
		break;
	}

	//dealloc
	if (occlusionQuery)
	{
		glDeleteQueries(1, &occlusionQuery);
	}
}
//----------------------------------------------------------------------------
void CausticsApp::Terminate()
{
	// Release all resources.

	mLight = 0;

	mReceiverGBuffer = 0;
	mReceiverNormalTexture = 0;
	mReceiverPositionTexture = 0;
	mReceiverColorTexture = 0;
	mReceiverDepthTexture = 0;
}
//----------------------------------------------------------------------------
void CausticsApp::ProcessInput()
{
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
		mCausticsScreenQuad->RefractionIndex -= 0.01f;
// 		mSSDOTempResultQuad->TempTexture = mSSDOTexture;
// 		mShowMode = SM_SSDO;
	}

	if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
	{
		mCausticsScreenQuad->RefractionIndex += 0.01f;
		// 		mSSDOTempResultQuad->TempTexture = mSSDOTexture;
		// 		mShowMode = SM_SSDO;
	}
	if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS)
	{
		mCausticsScreenQuad->RefractionIndex = 0.95f;
		// 		mSSDOTempResultQuad->TempTexture = mSSDOTexture;
		// 		mShowMode = SM_SSDO;
	}
}