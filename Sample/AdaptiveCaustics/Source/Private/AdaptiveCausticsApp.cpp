#include "AdaptiveCausticsApp.h"
#include "InformationPanel.h"
#include <glfw3.h>

using namespace RTGI;
using namespace RTGI::GUIFramework;

//----------------------------------------------------------------------------
AdaptiveCausticsApp::AdaptiveCausticsApp(int width, int height)
{
	Width = width;
	Height = height;
	Title = "Adaptive Caustics demo";
}
//----------------------------------------------------------------------------
AdaptiveCausticsApp::~AdaptiveCausticsApp()
{
}
//----------------------------------------------------------------------------
void AdaptiveCausticsApp::InitializeTextures()
{
	//mTextures.mReceiverPositionTexture;
}
//----------------------------------------------------------------------------
void AdaptiveCausticsApp::InitializeScene()
{

}
//----------------------------------------------------------------------------
void AdaptiveCausticsApp::InitializeFBO()
{

}
//----------------------------------------------------------------------------
void AdaptiveCausticsApp::Initialize(GPUDevice* device)
{
	mFBO.backgroundObjectsPositionLightSpace;
	mFBO.refractorNormalFrontBackLightSpace;
	// Create information panel.
	int screenX, screenY;
	glfwGetWindowPos(Window, &screenX, &screenY);
	InformationPanel^ infoPanel = gcnew InformationPanel();
	infoPanel->Show();
	infoPanel->SetDesktopLocation(screenX + Width + 12, screenY - 30);
	// Create GUI elements.
	InformationPanel::GetInstance()->AddListener(this);
	int infoStartY = 20;
	int infoIncY = 20;
	InformationPanel::GetInstance()->AddTimingLabel("Scene Voxelization Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Scene Shadow Pass", 16, infoStartY);
	infoStartY += infoIncY;

	/*
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


	// Frame buffer for caustics map
	mCausticsMapTexture = new Texture2D();
	mCausticsMapDepthTexture = new Texture2D();
	mCausticsMapTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mCausticsMapDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mCausticsMapGBuffer = new FrameBuffer(mDevice);
	Texture* CausticsMapColorTextures[1] = { mCausticsMapTexture };
	mCausticsMapGBuffer->SetRenderTargets(1, CausticsMapColorTextures, mCausticsMapDepthTexture);

	// Frame buffer for horizontal blurred caustics map
	mBlurredCausticsMapTexture = new Texture2D();
	mBlurredCausticsMapDepthTexture = new Texture2D();
	mBlurredCausticsMapTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mBlurredCausticsMapDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mBlurredCausticsMapGBuffer = new FrameBuffer(mDevice);
	Texture* BlurredCausticsMapColorTextures[1] = { mBlurredCausticsMapTexture };
	mBlurredCausticsMapGBuffer->SetRenderTargets(1, BlurredCausticsMapColorTextures, mBlurredCausticsMapDepthTexture);

	// Frame buffer for final blurred caustics map
	mBlurredCausticsMapTexture2 = new Texture2D();
	mBlurredCausticsMapDepthTexture2 = new Texture2D();
	mBlurredCausticsMapTexture2->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mBlurredCausticsMapDepthTexture2->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mBlurredCausticsMapGBuffer2 = new FrameBuffer(mDevice);
	Texture* BlurredCausticsMapColorTextures2[1] = { mBlurredCausticsMapTexture2 };
	mBlurredCausticsMapGBuffer2->SetRenderTargets(1, BlurredCausticsMapColorTextures2, mBlurredCausticsMapDepthTexture2);

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
	mCausticsScreenQuad->BlurredCausticsMapTexture = mBlurredCausticsMapTexture;

	// Final Render receiver
	mCausticsScreenQuad->BlurredCausticsMapTexture2 = mBlurredCausticsMapTexture2;
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
	*/
}

//----------------------------------------------------------------------------
void AdaptiveCausticsApp::FrameFunc()
{

}
//----------------------------------------------------------------------------
void AdaptiveCausticsApp::Terminate()
{
	// Release all resources.
	mLight = 0;
}
//----------------------------------------------------------------------------
void AdaptiveCausticsApp::ProcessInput()
{
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
// 		mSSDOTempResultQuad->TempTexture = mSSDOTexture;
// 		mShowMode = SM_SSDO;
	}
}
