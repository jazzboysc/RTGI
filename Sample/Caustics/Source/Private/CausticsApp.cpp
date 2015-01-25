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
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width / (float)Height, 2.0f, 50.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));
	mLight = new Light;
	mLight->SetLocation(vec3(0.0f, 0.0f, 0.0f));

	ShaderProgramInfo SICaustics;
	SICaustics += "Caustics/causticsDeferredLighting.vert";
	SICaustics += "Caustics/causticsDeferredLighting.frag";
	auto mtCaustics = new MaterialTemplate(new Technique(new Pass(SICaustics)));
	
	// Create MRT textures.
	mPositionTexture = new Texture2D();
	mNormalTexture = new Texture2D();
	mColorTexture = new Texture2D();
	mDepthTexture = new Texture2D();
	mPositionTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mNormalTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mColorTexture->CreateRenderTarget(mDevice, Width, Height, BF_RGBF);
	mDepthTexture->CreateRenderTarget(mDevice, Width, Height, BF_Depth);

	// Create G-buffer.
	mGBuffer = new FrameBuffer(mDevice);
	Texture* colorTextures[3] = { mPositionTexture, mNormalTexture, mColorTexture };
	mGBuffer->SetRenderTargets(3, colorTextures, mDepthTexture);

	// Create material templates.
	ShaderProgramInfo gbufferProgramInfo;
	gbufferProgramInfo += "Caustics/GBuffer.vert";
	gbufferProgramInfo += "Caustics/GBuffer.frag";
	auto mtGBuffer = new MaterialTemplate(new Technique(new Pass(gbufferProgramInfo)));

	ShaderProgramInfo gbufferCubeProgramInfo;
	gbufferCubeProgramInfo += "Caustics/GBufferCube.vert";
	gbufferCubeProgramInfo += "Caustics/GBufferCube.frag";
	auto mtGBufferCube = new MaterialTemplate(new Technique(new Pass(gbufferCubeProgramInfo)));

	mGround = new CausticsTriMesh(new Material(mtGBuffer), mMainCamera);
	mGround->LoadFromFile("ground.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource(mDevice);
	mGround->SetWorldTranslation(vec3(0.0f, -6.0f, 0.0f));
	mGround->MaterialColor = vec3(1.0f, 1.0f, 1.0f);

	vec2 tcoord00(0.0f, 0.0f);
	vec2 tcoord01(0.0f, 1.0f);
	vec2 tcoord10(1.0f, 0.0f);
	vec2 tcoord11(1.0f, 1.0f);

	// Create screen quad.
	Material* mat = new Material(mtCaustics);
	mCausticsScreenQuad = new CausticsScreenQuad(mat, mMainCamera);
	mCausticsScreenQuad->LoadFromFile("screenquad.ply");
	mCausticsScreenQuad->SetTCoord(0, tcoord00);
	mCausticsScreenQuad->SetTCoord(1, tcoord10);
	mCausticsScreenQuad->SetTCoord(2, tcoord11);
	mCausticsScreenQuad->SetTCoord(3, tcoord01);
	mCausticsScreenQuad->CreateDeviceResource(mDevice);
	mCausticsScreenQuad->PositionTexture = mPositionTexture;
	mCausticsScreenQuad->NormalTexture = mNormalTexture;
	mCausticsScreenQuad->ReflectanceTexture = mColorTexture;

	mCausticsScreenQuad->Light = mLight;

	auto mCubeMap = new TextureCube();
	mCubeMap->LoadFromFile(mDevice, 
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp");

	mPool = new CausticsCube(new Material(mtGBufferCube), mMainCamera);
	mPool->LoadFromFile("cube.ply");
	mPool->GenerateNormals();
	mPool->CreateDeviceResource(mDevice);
	//mPool->SetWorldTranslation(vec3(0.0f, 10.0f, 0.0f));
	mPool->SetWorldScale(vec3(1, -1, 1));
	mPool->MaterialColor = vec3(1, 1, 1);
	mPool->CubeTexture = mCubeMap;
//	Error |= !PoolSkyCubeMap.LoadTextureCubeMap(PoolSkyCubeMapFileNames);

	// ------------------------------------------------------------------------------------------------------------------------

// 	Error |= !WaterAddDropProgram.Load("wateradddrop.vs", "wateradddrop.fs");
// 	Error |= !WaterHeightMapProgram.Load("waterheightmap.vs", "waterheightmap.fs");
// 	Error |= !WaterNormalMapProgram.Load("waternormalmap.vs", "waternormalmap.fs");
// 	Error |= !PoolSkyProgram.Load("poolsky.vs", "poolsky.fs");
// 	Error |= !WaterProgram.Load("water.vs", "water.fs");
}
//----------------------------------------------------------------------------
void CausticsApp::DrawScene()
{
	mGround->Render(0, 0);
	mPool->Render(0, 0);
}
//----------------------------------------------------------------------------
void CausticsApp::FrameFunc()
{
	// Draw scene to G-buffer.
	mGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawScene();
	mGBuffer->Disable();

	// Draw final image.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mCausticsScreenQuad->Render(0, 0);

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