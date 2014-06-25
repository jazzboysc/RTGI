#include "RSMApp.h"
#include "RNG.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RSMApp::RSMApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("RSM demo")
{
	mShowMode = SM_Normal;
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

	// Create light projector.
	mLightProjector = new Camera;
	mLightProjector->SetFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 50.0f);
	mLightProjector->SetLookAt(vec3(10.0f, 23.0f, 32.0f), vec3(-5.0f, 0.0f, -15.0f),
		vec3(0.0f, 1.0f, 0.0f));
    
    // Create scene camera.
	mCamera = new Camera;
	mCamera->SetFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 50.0f);
	mCamera->SetLookAt(vec3(0.0f, 0.0f, 50.0f), vec3(0.0f, 0.0f, 0.0f), 
		vec3(0.0f, 1.0f, 0.0f));

	// Create light.
	mLight = new Light;
	mLight->SetProjector(mLightProjector);

	// Create material templates.
	Material* material = 0;
	Pass* passRSMBuffer = new Pass("vRSMBuffer.glsl", "fRSMBuffer.glsl");
    Pass* passIndirectLighting = new Pass("vIndirectLighting.glsl", "fIndirectLighting.glsl");
	Technique* techRSM = new Technique();
	techRSM->AddPass(passRSMBuffer);
    techRSM->AddPass(passIndirectLighting);
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
	mRSMTempResultQuad = new RSMTempScreenQuad(material, mCamera);
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
	mModel->SetWorldTranslation(vec3(-2.0f, 5.8f, -2.0f));
	mModel->MaterialColor = vec3(0.8f, 0.8f, 0.8f);

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

	material = new Material(mtRSM);
	mSphere = new RSMTriMesh(material, mCamera);
	mSphere->LoadFromFile("sphere.ply");
	mSphere->GenerateNormals();
	mSphere->CreateDeviceResource();
	mSphere->MaterialColor = vec3(1.0f, 0.0f, 0.0f);

	// Create sampling pattern texture.
	RandomNumberGenerator rng;
	float e1, e2;
	for( int i = 0; i < RSM_SAMPLE_COUNT; ++i )
	{
		e1 = rng.RandomFloat();
		e2 = rng.RandomFloat();
		randmoNumbers[2*i + 0] = e1 * sinf(2.0f * PI_SP *e2);
		randmoNumbers[2*i + 1] = e1 * cosf(2.0f * PI_SP *e2);
	}
	mSamplingPatternTexture = new Texture2D();
	mSamplingPatternTexture->LoadFromSystemMemory(GL_RG32F, RSM_SAMPLE_COUNT, 
		1, GL_RG, GL_FLOAT, (void*)randmoNumbers);
}
//----------------------------------------------------------------------------
void RSMApp::DrawSceneToRSMBuffer()
{
    mModel->SetCamera(mLightProjector);
	mModel->Render(0, 0);
    mLeftWall->SetCamera(mLightProjector);
	mLeftWall->Render(0, 0);
    mBackWall->SetCamera(mLightProjector);
	mBackWall->Render(0, 0);
    mGround->SetCamera(mLightProjector);
	mGround->Render(0, 0);
}
//----------------------------------------------------------------------------
void RSMApp::DrawScene()
{
 //   mModel->SetCamera(mCamera);
	//mModel->Render(0, 1);
 //   mLeftWall->SetCamera(mCamera);
	//mLeftWall->Render(0, 1);
 //   mBackWall->SetCamera(mCamera);
	//mBackWall->Render(0, 1);
 //   mGround->SetCamera(mCamera);
	//mGround->Render(0, 1);

	mSphere->MaterialColor = vec3(0.0f, 0.0f, 1.0f);
	mSphere->SetWorldScale(vec3(1.0f));
	mSphere->SetWorldTranslation(vec3(0.0f, 0.0f, 0.0f));
	mSphere->Render(0, 0);

	float r = 5.0f;
	mSphere->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
	for( int i = 0; i < RSM_SAMPLE_COUNT; ++i )
	{
		float x, y, d;
		x = r * randmoNumbers[2*i + 0];
		y = r * randmoNumbers[2*i + 1];
		d = sqrtf(x*x + y*y)*0.1f;

		mSphere->SetWorldScale(vec3(d, d, d));
		mSphere->SetWorldTranslation(vec3(x, y, 0.0f));
		mSphere->Render(0, 0);
	}
}
//----------------------------------------------------------------------------
void RSMApp::Run()
{
	// Draw scene to RSM-buffer.
	mRSMBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawSceneToRSMBuffer();
	mRSMBuffer->Disable();

	// Deferred lighting.
	//mDirectLightingBuffer->Enable();
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//mDirectLightingBuffer->Disable();

	// Draw final image.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if( mShowMode == SM_DirectLighting )
    {
        DrawScene();
    }
    else
    {
        mRSMTempResultQuad->Render(0, 0);
    }

	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void RSMApp::Terminate()
{
	// Release all resources.

    delete mLightProjector;
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
	mSphere = 0;
	mSamplingPatternTexture = 0;
}
//----------------------------------------------------------------------------
void RSMApp::OnKeyboard(unsigned char key, int x, int y)
{
	switch( key )
	{
	case '1':
		mShowMode = SM_Position;
        mRSMTempResultQuad->ShowMode = 1;
		mRSMTempResultQuad->TempTexture = mPositionTexture;
		break;

	case '2':
		mShowMode = SM_Normal;
        mRSMTempResultQuad->ShowMode = 2;
        mRSMTempResultQuad->TempTexture = mNormalTexture;
		break;

	case '3':
		mShowMode = SM_Flux;
		mRSMTempResultQuad->ShowMode = 3;
		mRSMTempResultQuad->TempTexture = mFluxTexture;
		break;

	case '4':
		mShowMode = SM_Depth;
        mRSMTempResultQuad->ShowMode = 4;
        mRSMTempResultQuad->TempTexture = mDepthTexture;
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