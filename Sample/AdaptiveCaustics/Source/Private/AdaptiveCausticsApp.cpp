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
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width / (float)Height, 0.1f, 50.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 0.0f, 5.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));

	mLightProjector = new Camera;
	mLightProjector->SetPerspectiveFrustum(75.0f, (float)Width / (float)Height, 1.0f, 50.0f);
	mLightProjector->SetLookAt(vec3(-0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));

	mLight = new Light;
	mLight->SetProjector(mLightProjector);
	mLight->Color = vec3(0.9f, 0.9f, 0.7f);

	mTimer = new GPUTimer();
	mTimer->CreateDeviceResource();

	// Pass: Render light space position of receiver geometry
	ShaderProgramInfo PI_outputEyeSpacePosition;
	PI_outputEyeSpacePosition
		<< "AdaptiveCaustics/outputEyeSpacePosition.vert"
		<< "AdaptiveCaustics/outputEyeSpacePosition.frag";
	/*
	// Cubemap variation
	ShaderProgramInfo PI_CubeReceiverPosition;
	PI_CubeReceiverPosition
		<< "AdaptiveCaustics/CausticsResourceCubeReceiver.vert"
		<< "AdaptiveCaustics/CausticsResourceCubeReceiver.frag";
	*/

	// Pass: Render light space front and back normal of refractor geometry
	ShaderProgramInfo PI_FrontBackNormals;
	PI_FrontBackNormals
		<< "AdaptiveCaustics/renderFrontAndBackNormals.vert"
		<< "AdaptiveCaustics/renderFrontAndBackNormals.geom"
		<< "AdaptiveCaustics/renderFrontAndBackNormals.frag";



	auto mtCausticsResourceReceiver = new MaterialTemplate(
		new Technique(new Pass(PI_outputEyeSpacePosition)));


	auto mtCausticsResourceRefractor = new MaterialTemplate(
		new Technique(new Pass(PI_FrontBackNormals)));


	// Receiver meshes
	auto mCubeMap = new TextureCube();
	mCubeMap->LoadFromFile(mDevice,
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp",
		"Textures/pool.bmp");

	/*
	auto mtCausticsResourceReceiverCubeReceiver = new MaterialTemplate(
		new Technique(new Pass(PI_CubeReceiverPosition)));
	*/

	mScene.pool = new AdaptiveCausticsCube(
		new Material(mtCausticsResourceReceiver), mMainCamera);
	mScene.pool->LoadFromPLYFile("cube.ply");
	mScene.pool->GenerateNormals();
	mScene.pool->CreateDeviceResource(mDevice);
	mScene.pool->SetWorldTranslation(vec3(0.0f, 0.0f, 0.0f));
	mScene.pool->SetWorldScale(vec3(1, -1, 1));
	mScene.pool->MaterialColor = vec3(1, 1, 1);
	mScene.pool->CubeTexture = mCubeMap;
	
	mScene.ground = new AdaptiveCausticsTriMesh(
		new Material(mtCausticsResourceReceiver), mMainCamera);
	mScene.ground->LoadFromPLYFile("ground.ply");
	mScene.ground->GenerateNormals();
	mScene.ground->CreateDeviceResource(mDevice);
	mScene.ground->SetWorldTranslation(vec3(0.0f, -6.0f, 0.0f));
	mScene.ground->MaterialColor = vec3(1.0f, 1.0f, 1.0f);

	// Refractor mesh
	mScene.mesh = new AdaptiveCausticsTriMesh(
		new Material(mtCausticsResourceRefractor), mMainCamera);
	mScene.mesh->LoadFromPLYFile("dragon_s.ply");
	mScene.mesh->GenerateNormals();
	mScene.mesh->CreateDeviceResource(mDevice);
	mScene.mesh->SetWorldTransform(rotate(mat4(), radians(30.0f), vec3(0, 1, 0)));
	mScene.mesh->SetWorldTranslation(vec3(0.0f, -0.8f, 0.0f));
	mScene.mesh->SetWorldScale(vec3(3.0f));
	mScene.mesh->MaterialColor = vec3(1.0f, 1.0f, 1.0f);

	// Render sets
	mScene.receiver = new RenderSet();
	mScene.receiver->AddRenderObject(mScene.pool);
	mScene.receiver->AddRenderObject(mScene.ground);

	mScene.refractor = new RenderSet();
	mScene.refractor->AddRenderObject(mScene.mesh);

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
	InformationPanel::GetInstance()->AddTimingLabel("Receiver Light Space Position Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Refractor Light Space Front Normal Pass", 16, infoStartY);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddTimingLabel("Refractor Light Space Back Normal Pass", 16, infoStartY);
	infoStartY += infoIncY;

	infoStartY = 20;
	InformationPanel::GetInstance()->AddRadioButton("Receiver Light Space Position", 16, infoStartY, 60, 20, true);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Refractor Light Space Front Normal", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddRadioButton("Refractor Light Space Back Normal", 16, infoStartY, 60, 20, false);
	infoStartY += infoIncY;
	InformationPanel::GetInstance()->AddCheckBox("Show Direct Shadow", 16, infoStartY, 60, 20, true);

	infoPanel->SetTimingLabelValue("Debug A", 0.01);




	
	mReceiverResourceRenderer = new ReceiverResourceRenderer(device, mScene.receiver);
	ReceiverResourceDesc receiverResourceDesc;
	receiverResourceDesc.Width = this->Width;
	receiverResourceDesc.Height = this->Height;
	receiverResourceDesc.ReceiverPositionFormat = BF_RGBAF;
	mReceiverResourceRenderer->CreateCausticsResource(&receiverResourceDesc);
	mReceiverResourceRenderer->SetTimer(mTimer);

	mRefractorResourceRenderer = new RefractorResourceRenderer(device, mScene.refractor);
	RefractorResourceDesc refractorResourceDesc;
	refractorResourceDesc.Width = this->Width;
	refractorResourceDesc.Height = this->Height;
	refractorResourceDesc.RefractorFrontNormalFormat = BF_RGBAF;
	mRefractorResourceRenderer->CreateCausticsResource(&refractorResourceDesc);
	mRefractorResourceRenderer->SetTimer(mTimer);

	mVisualizer = new Visualizer(device);
	mVisualizer->Initialize(device, mReceiverResourceRenderer,
		mRefractorResourceRenderer,
		mRefractorResourceRendererBack,
		mMainCamera);
	mVisualizer->SetTimer(mTimer);

}

//----------------------------------------------------------------------------
void AdaptiveCausticsApp::FrameFunc()
{
	InformationPanel^ infoPanel = InformationPanel::GetInstance();
	static double workLoad;
	static double totalWorkLoad;
	totalWorkLoad = 0.0;

	// Resource gathering pass
	mReceiverResourceRenderer->Render(0, SMP_Resource, mLightProjector);
	workLoad = mReceiverResourceRenderer->GetTimeElapsed();
	totalWorkLoad += workLoad;
	infoPanel->SetTimingLabelValue("Receiver Light Space Position Pass", workLoad);

	mRefractorResourceRenderer->Render(0, SMP_Resource, mLightProjector);
	workLoad = mRefractorResourceRenderer->GetTimeElapsed();
	totalWorkLoad += workLoad;
	infoPanel->SetTimingLabelValue("Refractor Light Space Front Normal Pass", workLoad);

// 	mRefractorResourceRendererBack->Render(0, SMP_Resource, mLightProjector);
// 	workLoad = mRefractorResourceRendererBack->GetTimeElapsed();
// 	totalWorkLoad += workLoad;
// 	infoPanel->SetTimingLabelValue("Refractor Light Space Back Normal Pass", workLoad);

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

	if (radioButton->Name == "Receiver Light Space Position")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_ReceiverLightSpacePosition);
	}

	if (radioButton->Name == "Refractor Light Space Front Normal")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_RefractorLightSpaceFrontNorm);
	}

	if (radioButton->Name == "Refractor Light Space Back Normal")
	{
		mVisualizer->SetShowMode(Visualizer::eSM_RefractorLightSpaceBackNorm);
	}

}

//----------------------------------------------------------------------------
void AdaptiveCausticsApp::OnCheckBoxClick(System::Object^ sender, System::EventArgs^ e)
{
	CheckBox^ checkBox = (CheckBox^)sender;

	if (checkBox->Name == "Show Direct Shadow")
	{
	}

	//mIndirectLightingRenderer->VPLVisibilityTest(checkBox->Checked);
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
