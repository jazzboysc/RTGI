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
	// Create camera and light
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width / (float)Height, 1.0f, 50.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	
	auto mLightCamera = new Camera;
	mLightCamera->SetPerspectiveFrustum(75.0f, (float)Width / (float)Height, 1.0f, 50.0f);
	mLightCamera->SetLookAt(vec3(0.0f, 0.65f, 0.0f), vec3(0.0f, -1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));

	mLight = new Light;
	mLight->SetProjector(mLightCamera);



	ShaderProgramInfo SIReceiver;
	SIReceiver << "Caustics/receiverMesh.vert";
	SIReceiver << "Caustics/receiverMesh.frag";
	auto mtReceiver = new MaterialTemplate(new Technique(new Pass(SIReceiver)));
	
	// G-buffer for receiver object
	mRecvPositionTexture = new Texture2D();
	mRecvNormalTexture = new Texture2D();
	mRecvColorTexture = new Texture2D();
	mRecvDepthTexture = new Texture2D();
	mRecvPositionTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mRecvNormalTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mRecvColorTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mRecvDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mRecvGBuffer = new FrameBuffer(mDevice);
	Texture* RecvColorTextures[3] = { mRecvPositionTexture, mRecvNormalTexture, mRecvColorTexture };
	mRecvGBuffer->SetRenderTargets(3, RecvColorTextures, mRecvDepthTexture);

	// G-buffer for refractive object
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

	// G-buffer for final image
	mCausticsIntersectionPositionTexture = new Texture2D();
	mCausticsIntersectionPositionTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mCausticsGBuffer = new FrameBuffer(mDevice);
	Texture* IntersectionColorTextures[1] = { mCausticsIntersectionPositionTexture };
	mCausticsGBuffer->SetRenderTargets(1, IntersectionColorTextures);

	// G-buffer for final image
	mPositionTexture = new Texture2D();
	mNormalTexture = new Texture2D();
	mColorTexture = new Texture2D();
	mDepthTexture = new Texture2D();
	mPositionTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mNormalTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mColorTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);
	mGBuffer = new FrameBuffer(mDevice);
	Texture* colorTextures[3] = { mPositionTexture, mNormalTexture, mColorTexture };
	mGBuffer->SetRenderTargets(3, colorTextures, mDepthTexture);

	// Create material templates.








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
	mMesh->SetWorldTranslation(vec3(0.0f, -0.65f, 0.0f));
	mMesh->SetWorldScale(vec3(5.0f));
	mMesh->MaterialColor = vec3(1.5f, 1.5f, 1.5f);


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
	mPool->SetWorldScale(vec3(1, -1, 1));
	mPool->MaterialColor = vec3(1, 1, 1);
	mPool->CubeTexture = mCubeMap;
	mPool->VertexSplattingTexture = mRefracPositionTexture;
	mPool->Light = mLight;

	// caustics map processor
	vec2 tcoord00(0.0f, 0.0f);
	vec2 tcoord01(0.0f, 1.0f);
	vec2 tcoord10(1.0f, 0.0f);
	vec2 tcoord11(1.0f, 1.0f);
	ShaderProgramInfo SICausticsMapIntersect;
	SICausticsMapIntersect << "Caustics/CausticsMap.vert";
	SICausticsMapIntersect << "Caustics/CausticsMap.frag";
	ShaderProgramInfo SICausticsMapLighting;
	SICausticsMapLighting << "Caustics/CausticsDeferredLighting.vert";
	SICausticsMapLighting << "Caustics/CausticsDeferredLighting.frag";
	auto mtCausticsMap = new MaterialTemplate(new Technique({
		new Pass(SICausticsMapIntersect),
		new Pass(SICausticsMapLighting) }));

	mCausticsScreenQuad = new CausticsScreenQuad(new Material(mtCausticsMap), mMainCamera);
	mCausticsScreenQuad->LoadFromFile("screenquad.ply");
	mCausticsScreenQuad->SetTCoord(0, tcoord00);
	mCausticsScreenQuad->SetTCoord(1, tcoord10);
	mCausticsScreenQuad->SetTCoord(2, tcoord11);
	mCausticsScreenQuad->SetTCoord(3, tcoord01);
	mCausticsScreenQuad->CreateDeviceResource(mDevice);
	// Pass 0
	mCausticsScreenQuad->PositionTexture = mPositionTexture;
	mCausticsScreenQuad->NormalTexture = mNormalTexture;
	mCausticsScreenQuad->ReflectanceTexture = mColorTexture;
	mCausticsScreenQuad->ReracterPositionTexture = mRefracPositionTexture;
	mCausticsScreenQuad->ReracterNormalTexture = mRefracNormalTexture;
	mCausticsScreenQuad->ReceiverPositionTexture = mRecvPositionTexture;
	mCausticsScreenQuad->RefractionIndex = 1.0;
	mCausticsScreenQuad->CubeTexture = mCubeMap;
	mCausticsScreenQuad->Light = mLight;
	// Pass 1
	mCausticsScreenQuad->CausticsMapsResolution = 10000;
	mCausticsScreenQuad->IntersectionPositionTexture = mCausticsIntersectionPositionTexture;
}

//----------------------------------------------------------------------------
void CausticsApp::DrawReceiverLightPoV()
{
	mGround->SetCamera(mLight->GetProjector());
	mPool->SetCamera(mLight->GetProjector());
	mGround->Render(0, 0);
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

void CausticsApp::DrawScene()
{
	mGround->Render(0, 0);
	mPool->Render(0, 0);
	mMesh->Render(0, 0);
}
//----------------------------------------------------------------------------
void CausticsApp::FrameFunc()
{
	// Draw Receiver to G-buffer from Light's PoV.
	mRecvGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawReceiverLightPoV();
	mRecvGBuffer->Disable();

	// Draw Refractive object to G-buffer from Light's PoV.
	mRefracGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawRefracterLightPoV();
	mRefracGBuffer->Disable();
	
	// Draw Receiver Camera pov
	mGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawReceiverCameraPoV();
	mGBuffer->Disable();

	// Draw intersection points
	mCausticsGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mCausticsScreenQuad->Render(0, 0);
	mCausticsGBuffer->Disable();

	// Draw final image
	mCausticsScreenQuad->Render(0, 1);

	switch (mShowMode)
	{
		break;
	default:
		break;
	}
}
//----------------------------------------------------------------------------
void CausticsApp::Terminate()
{
	// Release all resources.

	mLight = 0;

	mGBuffer = 0;
	mNormalTexture = 0;
	mPositionTexture = 0;
	mColorTexture = 0;
	mDepthTexture = 0;
}
//----------------------------------------------------------------------------
void CausticsApp::ProcessInput()
{
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
// 		mSSDOTempResultQuad->TempTexture = mSSDOTexture;
// 		mShowMode = SM_SSDO;
	}
}