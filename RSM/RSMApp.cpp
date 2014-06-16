#include "RSMApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMApp::RSMApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("RSM demo")
{
	mShowMode = SM_Depth;
}
//----------------------------------------------------------------------------
RSMApp::~RSMApp()
{
}
//----------------------------------------------------------------------------
void RSMApp::Initialize()
{
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.5f;
	glClearColor(color, color, color, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create camera.
	mCamera = new Camera;
	mCamera->SetFrustum(45.0f, (float)mWidth/(float)mHeight, 0.1f, 100.0f);
	mCamera->SetLookAt(vec3(10.0f, 23.0f, 32.0f), vec3(-5.0f, 0.0f, -15.0f), 
		vec3(0.0f, 1.0f, 0.0f));

	// Create light.
	mLight = new Light;
	mLight->Location = vec3(0.0f, 10.0f, 5.0f);

	// Create material templates.
	Material* material = 0;
	Pass* passRSMBuffer = new Pass("vRSMBuffer.glsl", "fRSMBuffer.glsl");
	Technique* techRSM = new Technique();
	techRSM->AddPass(passRSMBuffer);
	MaterialTemplate* mtRSM = new MaterialTemplate();
	mtRSM->AddTechnique(techRSM);

	Pass* passRSMTemp = new Pass("vRSMTemp.glsl", "fRSMTemp.glsl");
	Technique* techRSMTemp = new Technique();
	techRSMTemp->AddPass(passRSMTemp);
	MaterialTemplate* mtRSMTemp = new MaterialTemplate();
	mtRSMTemp->AddTechnique(techRSMTemp);

	//Pass* passDirectLighting = new Pass("vDirectLighting.glsl", "fDirectLighting.glsl");
	//Technique* techDirectLighting = new Technique();
	//techDirectLighting->AddPass(passDirectLighting);
	//MaterialTemplate* mtDirectLighting = new MaterialTemplate();
	//mtDirectLighting->AddTechnique(techDirectLighting);

	// Create MRT textures.
	mPositionTexture = new Texture2D();
	mPositionTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::RTF_RGBF);
	mNormalTexture = new Texture2D();
	mNormalTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::RTF_RGBF);
	mFluxTexture = new Texture2D();
	mFluxTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::RTF_RGBF);
	mDepthTexture = new Texture2D();
	mDepthTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::RTF_Depth);

	// Create RSM-buffer.
	Texture2D* colorTextures[3] = {mPositionTexture, mNormalTexture, mFluxTexture};
	mRSMBuffer = new FrameBuffer();
	mRSMBuffer->SetRenderTargets(3, colorTextures, mDepthTexture);

	// Create direct lighting render target.
	mDirectLightingTexture = new Texture2D();
	mDirectLightingTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::RTF_RGBF);

	// Create direct lighting framebuffer.
	Texture2D* directLightingTexture[1] = {mDirectLightingTexture};
	mDirectLightingBuffer = new FrameBuffer();
	mDirectLightingBuffer->SetRenderTargets(1, directLightingTexture, mDepthTexture);

	// Create RSM temp result screen quad.
	material = new Material(mtRSMTemp);
	mRSMTempResultQuad = new RSMTempScreenQuad(material);
	mRSMTempResultQuad->LoadFromFile("screenquad.ply");
	mRSMTempResultQuad->SetTCoord(0, vec2(0.0f, 0.0f));
	mRSMTempResultQuad->SetTCoord(1, vec2(1.0f, 0.0f));
	mRSMTempResultQuad->SetTCoord(2, vec2(1.0f, 1.0f));
	mRSMTempResultQuad->SetTCoord(3, vec2(0.0f, 1.0f));
	mRSMTempResultQuad->CreateDeviceResource();
	mRSMTempResultQuad->TempTexture = mNormalTexture;
    
	// Create scene.
	mat4 rotM;
	material = new Material(mtRSM);
	mModel = new RSMTriMesh(material, mCamera);
	mModel->LoadFromFile("beethoven.ply");
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	rotM = RotateY(90.0f);
	mModel->SetWorldTransform(rotM);
	mModel->SetWorldTranslation(vec3(-2.0f, 5.0f, -2.0f));
	mModel->MaterialColor = vec3(1.0f, 1.0f, 1.0f);

	material = new Material(mtRSM);
	mGround = new RSMTriMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
	mGround->MaterialColor = vec3(0.0f, 0.0f, 1.0f);

	material = new Material(mtRSM);
	mBackWall = new RSMTriMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
	mBackWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);

	material = new Material(mtRSM);
	mLeftWall = new RSMTriMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
	mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
}
//----------------------------------------------------------------------------
void RSMApp::DrawScene()
{
	mModel->Render();
	mLeftWall->Render();
	mBackWall->Render();
	mGround->Render();
}
//----------------------------------------------------------------------------
void RSMApp::Run()
{
	// Draw scene to RSM-buffer.
	mRSMBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawScene();
	mRSMBuffer->Disable();

	// Deferred lighting.
	//mDirectLightingBuffer->Enable();
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//mDirectLightingBuffer->Disable();

	// Draw final image.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mRSMTempResultQuad->Render();

	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void RSMApp::Terminate()
{
	// Release all resources.

	delete mCamera;
	mLight = 0;
	mGround = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mModel = 0;

	mRSMBuffer = 0;
	mNormalTexture = 0;
	mPositionTexture = 0;
	mFluxTexture = 0;
	mDepthTexture = 0;

	mDirectLightingBuffer = 0;
	mDirectLightingTexture = 0;

	mRSMTempResultQuad = 0;
}
//----------------------------------------------------------------------------
void RSMApp::OnKeyboard(unsigned char key, int x, int y)
{
	switch( key )
	{
	case '1':
		mShowMode = SM_Position;
		break;

	case '2':
		mShowMode = SM_Normal;
		break;

	case '3':
		mShowMode = SM_Flux;
		break;

	case '4':
		mShowMode = SM_Depth;
		break;

	case '5':
		mShowMode = SM_DirectLighting;

	default:
		break;
	}
}
//----------------------------------------------------------------------------
void RSMApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void RSMApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void RSMApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void RSMApp::OnReshape(int x, int y)
{
	mWidth = x;
	mHeight = y;
}
//----------------------------------------------------------------------------