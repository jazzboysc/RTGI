#include "AdaptiveCausticsApp.h"
#include "InformationPanel.h"
#include "LightMesh.h"
#include <glfw3.h>

using namespace RTGI;
using namespace RTGI::GUIFramework;

#define DEFAULT_SHOWMODE Visualizer::eSM_CausticMapSplat
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
#pragma region Scene Misc
	// Init things
	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Create camera and light
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width / (float)Height, 0.01f, 150.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));

	mLightProjector = new Camera;
	mLightProjector->SetPerspectiveFrustum(45.0f, (float)Width / (float)Height, 0.01f, 25.0f);
	mLightProjector->SetLookAt(vec3(-1.5f, -0.5f, 0.0f), vec3(0.0f, -1.7f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
	mLight = new Light;
	mLight->SetProjector(mLightProjector);
	mLight->Intensity = vec3(1.0f);

	mLightManager = new LightManager();
	mLightManager->AddPointLight(mLight);
	mLightManager->CreateLightBuffer(mDevice);

	mTimer = new GPUTimer();
	mTimer->CreateDeviceResource();
#pragma endregion Scene Misc

#pragma region Init LightMesh
	// Create light.
	ShaderProgramInfo PI_LightMesh;
	PI_LightMesh
		<< "AdaptiveCaustics/LightMesh.vert"
		<< "AdaptiveCaustics/LightMesh.frag";

	auto mtLightMesh = new MaterialTemplate(
		new Technique({
		new Pass(PI_LightMesh) })
		);

	//*
	LightMesh* lightMesh = new LightMesh(new Material(mtLightMesh), mMainCamera);
	lightMesh->LoadFromPLYFile("square.ply");
	mat4 lightMeshScale = glm::scale(mat4(), vec3(0.05f));
	lightMesh->UpdateModelSpaceVertices(lightMeshScale);
	mat4 lightRotM = rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
	lightMesh->SetWorldTransform(lightRotM);
	lightMesh->SetTCoord(0, vec2(0.0f, 0.0f));
	lightMesh->SetTCoord(1, vec2(1.0f, 0.0f));
	lightMesh->SetTCoord(2, vec2(1.0f, 1.0f));
	lightMesh->SetTCoord(3, vec2(0.0f, 1.0f));
	lightMesh->CreateDeviceResource(device);

	lightMesh->LightMeshTexture = new Texture2D();
	lightMesh->LightMeshTexture->LoadPNGFromFile(mDevice, "Textures/pointLight.png");

	mLight->SetLightMesh(lightMesh);
	lightMesh->SetWorldTranslation(mLightProjector->GetLocation());
#pragma endregion Init LightMesh

#pragma region Shaders and Materials
	//*/
	// Pass: geometry buffer
	ShaderProgramInfo PI_GBuffer;
	PI_GBuffer
		<< "AdaptiveCaustics/GBuffer/GBuffer.vert"
		<< "AdaptiveCaustics/GBuffer/GBuffer.frag";
	ShaderProgramInfo PI_GBufferCube;
	PI_GBufferCube
		<< "AdaptiveCaustics/GBuffer/GBufferCube.vert"
		<< "AdaptiveCaustics/GBuffer/GBufferCube.frag";

	// Pass: Render light space position of receiver geometry
	ShaderProgramInfo PI_outputEyeSpacePosition;
	PI_outputEyeSpacePosition
		<< "AdaptiveCaustics/CausticsResource/RenderEyeSpacePosition.vert"
		<< "AdaptiveCaustics/CausticsResource/RenderEyeSpacePosition.frag";

	// Pass: Render light space front and back normal of refractor geometry
	ShaderProgramInfo PI_FrontBackNormals;
	PI_FrontBackNormals
		<< "AdaptiveCaustics/CausticsResource/RenderFrontAndBackNormals.vert"
		<< "AdaptiveCaustics/CausticsResource/RenderFrontAndBackNormals.geom"
		<< "AdaptiveCaustics/CausticsResource/RenderFrontAndBackNormals.frag";

	// Pass: Render Shadow Map for refractor
	ShaderProgramInfo PI_ShadowMap;
	PI_ShadowMap
		<< "AdaptiveCaustics/ParaboloidShadowmap/ShadowMap.vert"
		<< "AdaptiveCaustics/ParaboloidShadowmap/ShadowMap.ctrl"
		<< "AdaptiveCaustics/ParaboloidShadowmap/ShadowMap.eval"
		<< "AdaptiveCaustics/ParaboloidShadowmap/ShadowMap.geom"
		<< "AdaptiveCaustics/ParaboloidShadowmap/ShadowMap.frag";

	auto mtReceiverCausticsResourceCube = new MaterialTemplate(
		new Technique({
		new Pass(PI_GBufferCube),
		new Pass(PI_outputEyeSpacePosition),
		new Pass(PI_ShadowMap)})
		);

	auto mtReceiverCausticsResource = new MaterialTemplate(
		new Technique({
		new Pass(PI_GBuffer),
		new Pass(PI_outputEyeSpacePosition),
		new Pass(PI_ShadowMap) })
		);

	auto mtRefractorCausticsResource = new MaterialTemplate(
		new Technique({
		new Pass(PI_GBuffer),
		new Pass(PI_FrontBackNormals),
		new Pass(PI_ShadowMap)})
		);

	// Receiver meshes
	auto mCubeMap = new TextureCube();
	mCubeMap->LoadFromFile(mDevice,
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp");

	mScene.pool = new AdaptiveCausticsCube(
		new Material(mtReceiverCausticsResourceCube), mMainCamera);
	mScene.pool->LoadFromPLYFile("cube.ply");
	mScene.pool->GenerateNormals();
	mScene.pool->CreateDeviceResource(mDevice);
	mScene.pool->SetWorldTranslation(vec3(0.0f, 0.0f, 0.0f));
	mScene.pool->SetWorldScale(vec3(2, -2, 2));
	mScene.pool->MaterialColor = vec3(1, 1, 1);
	mScene.pool->CubeTexture = mCubeMap;
	
	mScene.ground = new AdaptiveCausticsTriMesh(
		new Material(mtReceiverCausticsResource), mMainCamera);
	mScene.ground->LoadFromPLYFile("ground.ply");
	mScene.ground->GenerateNormals();
	mScene.ground->CreateDeviceResource(mDevice);
	mScene.ground->SetWorldTranslation(vec3(0.0f, -6.0f, 0.0f));
	mScene.ground->MaterialColor = vec3(1.0f, 1.0f, 1.0f);

	// Refractor mesh
	mScene.mesh = new AdaptiveCausticsTriMesh(
		new Material(mtRefractorCausticsResource), mMainCamera);
	mScene.mesh->LoadFromPLYFile("dragon_s.ply");
	mScene.mesh->GenerateNormals();
	mScene.mesh->CreateDeviceResource(mDevice);
	mScene.mesh->SetWorldTransform(rotate(mat4(), radians(30.0f), vec3(0, 1, 0)));
	mScene.mesh->SetWorldTranslation(vec3(0.0f, -1.5f, 0.0f));
	mScene.mesh->SetWorldScale(vec3(6.0f));
    mScene.mesh->MaterialColor = vec3(1.0f, 1.0f, 1.0f);
	mScene.mesh->TessLevel = 1.0f;

	// Render sets
	mScene.receiver = new RenderSet();
	mScene.receiver->AddRenderObject(mScene.pool);
	//mScene.receiver->AddRenderObject(mScene.ground);

	mScene.refractor = new RenderSet();
	mScene.refractor->AddRenderObject(mScene.mesh);

	mScene.all = new RenderSet();
	mScene.all->AddRenderObject(mScene.pool);
	//mScene.all->AddRenderObject(mScene.ground);
	mScene.all->AddRenderObject(mScene.mesh);
#pragma endregion Shaders and Materials

#pragma region GUI
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
	InformationPanel::GetInstance()->AddTimingLabel("Receiver G-Buffer Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Refractor G-Buffer Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Light Space Receiver Position Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Light Space Refractor Normal Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Paraboloid Shadow Map Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Adaptive Caustic Map Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Direct Lighting Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Deferred Refraction Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Total", 16, infoStartY);
	infoStartY += infoIncY;

	infoStartY = 20;
	InformationPanel::GetInstance()->AddRadioButton("Receiver G-Buffer Position", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Receiver G-Buffer Normal", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Receiver G-Buffer Albedo", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Light Space Receiver Position", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Light Space Refractor Front Normal", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Light Space Refractor Back Normal", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Paraboloid Shadow Map", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Adaptive Caustic Map Traversal", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Adaptive Caustic Map Splat", 16, infoStartY, 60, 20, true);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddButton("Decrease Iteration", 30, infoStartY, 110, 24);
	InformationPanel::GetInstance()->AddButton("Increase Iteration", 145, infoStartY, 110, 24);
	infoStartY += infoIncY;
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Direct Lighting", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Deferred Refraction", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddCheckBox("Spin Mesh", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddCheckBox("Draw Debug Mipmap", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddCheckBox("Show Direct Shadow", 16, infoStartY, 60, 20, true);
#pragma endregion GUI




	// Step 1: Render resources needed for caustics computation
	glEnable(GL_CULL_FACE);
	mReceiverGBufferRenderer = new GBufferRenderer(device, mScene.receiver);
	GBufferDesc gBufferDesc;
	gBufferDesc.Width = this->Width;
	gBufferDesc.Height = this->Height;
	gBufferDesc.PositionFormat = BF_RGBA16F;
	gBufferDesc.PositionMipmap = false;
	gBufferDesc.NormalFormat = BF_RGBA16F;
	gBufferDesc.NormalMipmap = false;
	gBufferDesc.AlbedoFormat = BF_RGBA16F;
	gBufferDesc.AlbedoMipmap = false;
	mReceiverGBufferRenderer->CreateGBuffer(&gBufferDesc);
	mReceiverGBufferRenderer->SetTimer(mTimer);

	mRefractorGBufferRenderer = new GBufferRenderer(device, mScene.refractor);
	mRefractorGBufferRenderer->CreateGBuffer(&gBufferDesc);
	mRefractorGBufferRenderer->SetTimer(mTimer);

	mReceiverResourceRenderer = new ReceiverResourceRenderer(device, mScene.receiver);
	ReceiverResourceDesc receiverResourceDesc;
	receiverResourceDesc.Width = this->Width;
	receiverResourceDesc.Height = this->Height;
	receiverResourceDesc.ReceiverPositionFormat = BF_RGBA16F;
	receiverResourceDesc.ReceiverPositionMipmap = false;
	mReceiverResourceRenderer->CreateCausticsResource(&receiverResourceDesc);
	mReceiverResourceRenderer->SetTimer(mTimer);

	glDisable(GL_CULL_FACE);
	mRefractorResourceRenderer = new RefractorResourceRenderer(device, mScene.refractor);
	RefractorResourceDesc refractorResourceDesc;
	refractorResourceDesc.Width = 1024;
	refractorResourceDesc.Height = 1024;
	refractorResourceDesc.RefractorNormalFormat = BF_RGBA16F;
	refractorResourceDesc.RefractorFrontNormalMipmap = true;
	mRefractorResourceRenderer->CreateCausticsResource(&refractorResourceDesc);
	mRefractorResourceRenderer->SetTimer(mTimer);

	mRefractorNormalRenderer = new RefractorResourceRenderer(device, mScene.refractor);
	refractorResourceDesc.RefractorFrontNormalMipmap = false;
	refractorResourceDesc.Width = this->Width;
	refractorResourceDesc.Height = this->Height;
	mRefractorNormalRenderer->CreateCausticsResource(&refractorResourceDesc);
	mRefractorNormalRenderer->SetTimer(mTimer);

	glEnable(GL_CULL_FACE);
	mShadowMapRenderer = new ShadowMapRenderer(device, mScene.all);
	mShadowMapRenderer->CreateShadowMap(1024, 1024, BF_RGBA16F);
	mShadowMapRenderer->SetTimer(mTimer);

	mCausticMapRenderer = new CausticMapRenderer(device);
	CausticsMapDesc causticsMapDesc;
	causticsMapDesc.Width = 1024.0f;
	causticsMapDesc.Height = 1024.0f;
	causticsMapDesc.CausticsMapFormat = BF_RGBAF;
	//causticsMapDesc.CausticsMapMipmap = true;
	mCausticMapRenderer->Initialize(device, &causticsMapDesc,
		mReceiverResourceRenderer,
		mRefractorResourceRenderer,
		mMainCamera);
	mCausticMapRenderer->CreateCausticsResource(&causticsMapDesc);
	mCausticMapRenderer->SetTimer(mTimer);

	mDirectLightingRenderer = new DirectLightingRenderer(device);
	mDirectLightingRenderer->Initialize(mDevice, this->Width, this->Height, BF_RGBAF,
		mReceiverGBufferRenderer,
		mCausticMapRenderer,
		mReceiverResourceRenderer,
		mShadowMapRenderer);
	mDirectLightingRenderer->SetTimer(mTimer);

	mDeferredRefractionRenderer = new DeferredRefractionRenderer(device);
	mDeferredRefractionRenderer->Initialize(device, this->Width, this->Height, BF_RGBAF, mMainCamera,
		mReceiverGBufferRenderer,
		mRefractorGBufferRenderer,
		mReceiverResourceRenderer,
		mRefractorResourceRenderer,
		mRefractorNormalRenderer,
		mDirectLightingRenderer);
		mDeferredRefractionRenderer->SetTimer(mTimer);

	mVisualizer = new Visualizer(device);
	mVisualizer->Initialize(device,
		mReceiverGBufferRenderer,
		mReceiverResourceRenderer,
		mRefractorResourceRenderer,
		mShadowMapRenderer,
		mCausticMapRenderer,
		mDirectLightingRenderer,
		mDeferredRefractionRenderer,
		mMainCamera);
	mVisualizer->SetTimer(mTimer);
	mVisualizer->SetShowMode(DEFAULT_SHOWMODE);
}


//----------------------------------------------------------------------------
void AdaptiveCausticsApp::FrameFunc()
{
	InformationPanel^ infoPanel = InformationPanel::GetInstance();
	static double workLoad;
	static double totalWorkLoad;
	totalWorkLoad = 0.0;

	static float angle = 30.0f;
	if (bIsSpinningMesh)
	{
		angle -= 1.0f;
		mat4 rot;
		rot = rotate(mat4(), radians(angle), vec3(0, 1, 0));
		vec3 trans = mScene.mesh->GetWorldTranslation();
		mScene.mesh->SetWorldTransform(rot);
		mScene.mesh->SetWorldTranslation(trans);
	}

	mLightManager->SetLightBufferBindingPoint(1);
	mLightManager->UpdateLightBuffer();

//#define PROFILE_ENABLED

	// Resource gathering pass
 	mReceiverGBufferRenderer->Render(0, SMP_GBuffer, mMainCamera);
#ifdef PROFILE_ENABLED
	workLoad = mReceiverGBufferRenderer->GetTimeElapsed();
	totalWorkLoad += workLoad;
	infoPanel->SetTimingLabelValue("Receiver G-Buffer Pass", workLoad);
#endif // PROFILE_ENABLED

	mRefractorNormalRenderer->Render(0, SMP_Resource, mMainCamera);
#ifdef PROFILE_ENABLED
	workLoad = mRefractorNormalRenderer->GetTimeElapsed();
	totalWorkLoad += workLoad;
	infoPanel->SetTimingLabelValue("Refractor G-Buffer Pass", workLoad);
#endif // PROFILE_ENABLED

	mReceiverResourceRenderer->Render(0, SMP_Resource, mLightProjector);
#ifdef PROFILE_ENABLED
	workLoad = mReceiverResourceRenderer->GetTimeElapsed();
	totalWorkLoad += workLoad;
	infoPanel->SetTimingLabelValue("Light Space Receiver Position Pass", workLoad);
#endif // PROFILE_ENABLED

	mRefractorResourceRenderer->Render(0, SMP_Resource, mLightProjector);
#ifdef PROFILE_ENABLED
	workLoad = mRefractorResourceRenderer->GetTimeElapsed();
	totalWorkLoad += workLoad;
	infoPanel->SetTimingLabelValue("Light Space Refractor Normal Pass", workLoad);
#endif // PROFILE_ENABLED

	mShadowMapRenderer->Render(0, SMP_ShadowMap, mLightProjector);
#ifdef PROFILE_ENABLED
	workLoad = mShadowMapRenderer->GetTimeElapsed();
 	totalWorkLoad += workLoad;
 	infoPanel->SetTimingLabelValue("Paraboloid Shadow Map Pass", workLoad);
#endif // PROFILE_ENABLED

	mCausticMapRenderer->Render(0, 0/*SMP_AdaptiveCaustics*/, mLightProjector);
#ifdef PROFILE_ENABLED
	workLoad = mCausticMapRenderer->GetTimeElapsed();
	totalWorkLoad += workLoad;
	infoPanel->SetTimingLabelValue("Adaptive Caustic Map Pass", workLoad);
#endif // PROFILE_ENABLED

	mDirectLightingRenderer->Render();
#ifdef PROFILE_ENABLED
	workLoad = mDirectLightingRenderer->GetTimeElapsed();
	totalWorkLoad += workLoad;
	infoPanel->SetTimingLabelValue("Direct Lighting Pass", workLoad);
#endif // PROFILE_ENABLED

	mDeferredRefractionRenderer->Render();
#ifdef PROFILE_ENABLED
	workLoad = mDeferredRefractionRenderer->GetTimeElapsed();
	totalWorkLoad += workLoad;
	infoPanel->SetTimingLabelValue("Deferred Refraction Pass", workLoad);
#endif // PROFILE_ENABLED

	/*
	// OK.  Now that we have all the information to compute all the photons
	//    stored in our temporary FBOs, adaptively traverse the "mipmap" levels
	//    of our virtual photon buffer.  In other words, start at a 64x64
	//    resolution, compute all the photons, and then in each area where 
	//    the photons hit the refractor, adaptively subdivide (don't subdivide
	//    in other regions).
	data->fbo->causticMap[lightNum]->BindBuffer();
	data->fbo->causticMap[lightNum]->ClearBuffers();
	glLoadIdentity();
	scene->LightLookAtMatrix(lightNum);

	// The actual hierarchical traversal is given in code in utilRoutines.cpp.  This
	//    code traverses our photon hierarchy and outputs the correct photons as 
	//    splats into the current buffer (the causticMap[lightNum] enabled above)
	glBlendFunc(GL_ONE, GL_ONE);
	int startLevel = 6;  // The starting traversal level (2^6 = 64x64 photons)

	HierarchicalTransformFeedbackTraversalWithBatching(data->shader->adaptCaustic_traverse,
		data->shader->adaptCaustic_splat,
		startLevel,
		lightNum);

	// All right, we just created the caustic map, so do some housekeeping to make sure
	//    our rendering pipeline (see directories "Scene," "Objects" and "Materials") can 
	//    correctly render geometry using a caustic map.
	data->fbo->causticMap[lightNum]->UnbindBuffer();
	*/

	// Show rendering result.
	mVisualizer->Render(0, 0);
#ifdef PROFILE_ENABLED
	workLoad = mVisualizer->GetTimeElapsed();
	totalWorkLoad += workLoad;
	infoPanel->SetTimingLabelValue("Total", totalWorkLoad);
#endif // PROFILE_ENABLED

	// Post processing: add light mesh
	mLight->RenderLightMesh(0, 0);

}
//----------------------------------------------------------------------------
void AdaptiveCausticsApp::Terminate()
{
	// Release all resources.
	mLight = 0;
}

void AdaptiveCausticsApp::OnRadioButtonClick(System::Object^ sender, System::EventArgs^ e)
{
	RadioButton^ radioButton = (RadioButton^)sender;
	if (!mVisualizer)
	{
		return;
	}

	if (radioButton->Name == "Receiver G-Buffer Position")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_ReceiverGBufferPosition);
	}
	if (radioButton->Name == "Receiver G-Buffer Normal")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_ReceiverGBufferNormal);
	}
	if (radioButton->Name == "Receiver G-Buffer Albedo")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_ReceiverGBufferAlbedo);
	}
	if (radioButton->Name == "Light Space Receiver Position")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_ReceiverLightSpacePosition);
	}
	if (radioButton->Name == "Light Space Refractor Front Normal")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_RefractorLightSpaceFrontNorm);
	}
	if (radioButton->Name == "Light Space Refractor Back Normal")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_RefractorLightSpaceBackNorm);
	}
	if (radioButton->Name == "Paraboloid Shadow Map")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_RefractorShadow);
	}
	if (radioButton->Name == "Adaptive Caustic Map Traversal")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_CausticMapTraversal);
	}
	if (radioButton->Name == "Adaptive Caustic Map Splat")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_CausticMapSplat);
	}
	if (radioButton->Name == "Direct Lighting")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_DirectLighting);
	}
	if (radioButton->Name == "Deferred Refraction")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_DeferredRefraction);
	}

}

//----------------------------------------------------------------------------
void AdaptiveCausticsApp::OnCheckBoxClick(System::Object^ sender, System::EventArgs^ e)
{
	CheckBox^ checkBox = (CheckBox^)sender;

	if (checkBox->Name == "Show Direct Shadow")
	{
	}
	if (checkBox->Name == "Spin Mesh")
	{
		this->bIsSpinningMesh = checkBox->Checked;
	}
	if (checkBox->Name == "Draw Debug Mipmap")
	{
		this->mCausticMapRenderer->DrawDebugMipmap = checkBox->Checked;
	}
	


	//mIndirectLightingRenderer->VPLVisibilityTest(checkBox->Checked);
}
//----------------------------------------------------------------------------
void AdaptiveCausticsApp::OnButtonClick(System::Object^  sender,
	System::EventArgs^  e)
{
	Button^ button = (Button^)sender;

	if (button->Name == "Decrease Iteration")
	{
		mCausticMapRenderer->TraversalLevel--;

		if (mCausticMapRenderer->TraversalLevel == 0)
		{
			mCausticMapRenderer->TraversalLevel = 1;
		}
	}

#define MAX_ITERATION 9
	if (button->Name == "Increase Iteration")
	{
		mCausticMapRenderer->TraversalLevel++;

		if (mCausticMapRenderer->TraversalLevel > MAX_ITERATION)
		{
			mCausticMapRenderer->TraversalLevel = MAX_ITERATION;
		}
	}
}
//----------------------------------------------------------------------------
void AdaptiveCausticsApp::ProcessInput()
{
	if (glfwGetKey(Window, GLFW_KEY_0) == GLFW_PRESS)
	{
		mCausticMapRenderer->DebugMipmapLevel = 0;
	}
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
		mCausticMapRenderer->DebugMipmapLevel = 1;
	}
	if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
	{
		mCausticMapRenderer->DebugMipmapLevel = 2;
	}
	if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS)
	{
		mCausticMapRenderer->DebugMipmapLevel = 3;
	}
	if (glfwGetKey(Window, GLFW_KEY_4) == GLFW_PRESS)
	{
		mCausticMapRenderer->DebugMipmapLevel = 4;
	}
	if (glfwGetKey(Window, GLFW_KEY_5) == GLFW_PRESS)
	{
		mCausticMapRenderer->DebugMipmapLevel = 5;
	}
	if (glfwGetKey(Window, GLFW_KEY_6) == GLFW_PRESS)
	{
		mCausticMapRenderer->DebugMipmapLevel = 6;
	}
	if (glfwGetKey(Window, GLFW_KEY_7) == GLFW_PRESS)
	{
		mCausticMapRenderer->DebugMipmapLevel = 7;
	}
	if (glfwGetKey(Window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		mCausticMapRenderer->TraversalLevel++;

		if (mCausticMapRenderer->TraversalLevel > 4)
		{
			mCausticMapRenderer->TraversalLevel = 4;
		}
	}
	if (glfwGetKey(Window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		mCausticMapRenderer->TraversalLevel--;

		if (mCausticMapRenderer->TraversalLevel == 0)
		{
			mCausticMapRenderer->TraversalLevel = 1;
		}
	}
}
