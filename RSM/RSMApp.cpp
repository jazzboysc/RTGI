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
	mShowMode = SM_DeferredLighting;
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
	mLightProjector->SetPerspectiveFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 50.0f);
	mLightProjector->SetLookAt(vec3(10.0f, 23.0f, 32.0f), vec3(-7.0f, 0.0f, -12.0f),
		vec3(0.0f, 1.0f, 0.0f));
    
    // Create scene camera.
	mCamera = new Camera;
	mCamera->SetPerspectiveFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 50.0f);
	mCamera->SetLookAt(vec3(18.0f, 15.0f, 5.0f), vec3(0.0f, 6.0f, -2.0f),
        vec3(0.0f, 1.0f, 0.0f));

	// Create light.
	mLight = new Light;
	mLight->SetProjector(mLightProjector);

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo programInfo;
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex | ShaderType::ST_Fragment;
    programInfo.VShaderFileName = "vRSMBuffer.glsl";
    programInfo.FShaderFileName = "fRSMBuffer.glsl";
    Pass* passRSMBuffer = new Pass(programInfo);
    programInfo.VShaderFileName = "vRSMGBuffer.glsl";
    programInfo.FShaderFileName = "fRSMGBuffer.glsl";
    Pass* passRSMGBuffer = new Pass(programInfo);
	Technique* techRSM = new Technique();
	techRSM->AddPass(passRSMBuffer);
    techRSM->AddPass(passRSMGBuffer);
	MaterialTemplate* mtRSM = new MaterialTemplate();
	mtRSM->AddTechnique(techRSM);

    programInfo.VShaderFileName = "vRSMTemp.glsl";
    programInfo.FShaderFileName = "fRSMTemp.glsl";
    Pass* passRSMTemp = new Pass(programInfo);
	Technique* techRSMTemp = new Technique();
	techRSMTemp->AddPass(passRSMTemp);
	MaterialTemplate* mtRSMTemp = new MaterialTemplate();
	mtRSMTemp->AddTechnique(techRSMTemp);

    programInfo.VShaderFileName = "vRSMDeferredLighting.glsl";
    programInfo.FShaderFileName = "fRSMDeferredLighting.glsl";
    Pass* passDeferredLighting = new Pass(programInfo);
	Technique* techDeferredLighting = new Technique();
	techDeferredLighting->AddPass(passDeferredLighting);
	MaterialTemplate* mtDeferredLighting = new MaterialTemplate();
	mtDeferredLighting->AddTechnique(techDeferredLighting);

	// Create RSM-buffer MRT textures.
	mRSMPositionTexture = new Texture2D();
	mRSMPositionTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
	mRSMNormalTexture = new Texture2D();
	mRSMNormalTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
	mRSMFluxTexture = new Texture2D();
	mRSMFluxTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
	mRSMDepthTexture = new Texture2D();
	mRSMDepthTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_Depth);

	// Create RSM-buffer.
	Texture* rsmTextures[3] = {mRSMPositionTexture, mRSMNormalTexture, mRSMFluxTexture};
	mRSMBuffer = new FrameBuffer();
	mRSMBuffer->SetRenderTargets(3, rsmTextures, mRSMDepthTexture);
    
	// Create G-buffer MRT textures.
	mPositionTexture = new Texture2D();
	mPositionTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
	mNormalTexture = new Texture2D();
	mNormalTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
	mColorTexture = new Texture2D();
	mColorTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
	mDepthTexture = new Texture2D();
	mDepthTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_Depth);
	mIndirectLightingTexture = new Texture2D();
	mIndirectLightingTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
    
	// Create G-buffer.
	Texture* gbufferTextures[4] = {mPositionTexture, mNormalTexture, mColorTexture, mIndirectLightingTexture};
	mGBuffer = new FrameBuffer();
	mGBuffer->SetRenderTargets(4, gbufferTextures, mDepthTexture);
    
	// Create RSM sampling pattern texture.
	RandomNumberGenerator rng;
	float e1, e2;
	for( int i = 0; i < RSM_SAMPLE_COUNT; ++i )
	{
		e1 = rng.RandomFloat();
		e2 = rng.RandomFloat();
		randmoNumbers[3*i + 0] = e1 * sinf(2.0f * PI_SP *e2);
		randmoNumbers[3*i + 1] = e1 * cosf(2.0f * PI_SP *e2);
        randmoNumbers[3*i + 2] = e1 * e1;
	}
	mSamplingPatternTexture = new Texture2D();
	mSamplingPatternTexture->LoadFromSystemMemory(GL_RGB32F_ARB,
                                                  RSM_SAMPLE_COUNT, 1, GL_RGB, GL_FLOAT, (void*)randmoNumbers);
    
	// Create RSM temp result screen quad.
	material = new Material(mtRSMTemp);
	mRSMTempResultQuad = new RSMTempScreenQuad(material, mCamera);
	mRSMTempResultQuad->LoadFromFile("screenquad.ply");
	mRSMTempResultQuad->SetTCoord(0, vec2(0.0f, 0.0f));
	mRSMTempResultQuad->SetTCoord(1, vec2(1.0f, 0.0f));
	mRSMTempResultQuad->SetTCoord(2, vec2(1.0f, 1.0f));
	mRSMTempResultQuad->SetTCoord(3, vec2(0.0f, 1.0f));
	mRSMTempResultQuad->CreateDeviceResource();
	mRSMTempResultQuad->TempTexture = mRSMNormalTexture;
    
	// Create RSM deferred lighting screen quad.
	material = new Material(mtDeferredLighting);
	mRSMDeferredLightingQuad = new RSMDeferredLightingQuad(material, mLight);
	mRSMDeferredLightingQuad->LoadFromFile("screenquad.ply");
	mRSMDeferredLightingQuad->SetTCoord(0, vec2(0.0f, 0.0f));
	mRSMDeferredLightingQuad->SetTCoord(1, vec2(1.0f, 0.0f));
	mRSMDeferredLightingQuad->SetTCoord(2, vec2(1.0f, 1.0f));
	mRSMDeferredLightingQuad->SetTCoord(3, vec2(0.0f, 1.0f));
	mRSMDeferredLightingQuad->CreateDeviceResource();
    mRSMDeferredLightingQuad->PositionTexture = mPositionTexture;
    mRSMDeferredLightingQuad->NormalTexture = mNormalTexture;
    mRSMDeferredLightingQuad->ReflectanceTexture = mColorTexture;
    mRSMDeferredLightingQuad->IndirectLightingTexture = mIndirectLightingTexture;
    
	// Create scene.
    float RSMSamplingRadius = 0.05f;
	mat4 rotM;
	material = new Material(mtRSM);
	mModel = new RSMTriMesh(material, mCamera);
	mModel->LoadFromFile("beethoven.ply");
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	rotM = RotateY(90.0f);
	mModel->SetWorldTransform(rotM);
	mModel->SetWorldTranslation(vec3(-2.0f, 5.8f, -1.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);
    mModel->SampleRadius = RSMSamplingRadius;
    mModel->SampleCount = RSM_SAMPLE_COUNT;
    mModel->LightProjector = mLightProjector;
    mModel->RSMPositionTexture = mRSMPositionTexture;
    mModel->RSMNormalTexture = mRSMNormalTexture;
    mModel->RSMFluxTexture = mRSMFluxTexture;
    mModel->RSMSamplingPatternTexture = mSamplingPatternTexture;

	material = new Material(mtRSM);
	mGround = new RSMTriMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(0.25f, 0.25f, 0.75f);
    mGround->SampleRadius = RSMSamplingRadius;
    mGround->SampleCount = RSM_SAMPLE_COUNT;
    mGround->LightProjector = mLightProjector;
    mGround->RSMPositionTexture = mRSMPositionTexture;
    mGround->RSMNormalTexture = mRSMNormalTexture;
    mGround->RSMFluxTexture = mRSMFluxTexture;
    mGround->RSMSamplingPatternTexture = mSamplingPatternTexture;

	material = new Material(mtRSM);
	mBackWall = new RSMTriMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.25f, 0.75f, 0.25f);
    mBackWall->SampleRadius = RSMSamplingRadius;
    mBackWall->SampleCount = RSM_SAMPLE_COUNT;
    mBackWall->LightProjector = mLightProjector;
    mBackWall->RSMPositionTexture = mRSMPositionTexture;
    mBackWall->RSMNormalTexture = mRSMNormalTexture;
    mBackWall->RSMFluxTexture = mRSMFluxTexture;
    mBackWall->RSMSamplingPatternTexture = mSamplingPatternTexture;

	material = new Material(mtRSM);
	mLeftWall = new RSMTriMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(0.75f, 0.25f, 0.25f);
    mLeftWall->SampleRadius = RSMSamplingRadius;
    mLeftWall->SampleCount = RSM_SAMPLE_COUNT;
    mLeftWall->LightProjector = mLightProjector;
    mLeftWall->RSMPositionTexture = mRSMPositionTexture;
    mLeftWall->RSMNormalTexture = mRSMNormalTexture;
    mLeftWall->RSMFluxTexture = mRSMFluxTexture;
    mLeftWall->RSMSamplingPatternTexture = mSamplingPatternTexture;

	material = new Material(mtRSM);
	mSphere = new RSMTriMesh(material, mCamera);
	mSphere->LoadFromFile("sphere.ply");
	mSphere->GenerateNormals();
	mSphere->CreateDeviceResource();
	mSphere->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
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
    mModel->SetCamera(mCamera);
	mModel->Render(0, 1);
    mLeftWall->SetCamera(mCamera);
	mLeftWall->Render(0, 1);
    mBackWall->SetCamera(mCamera);
	mBackWall->Render(0, 1);
    mGround->SetCamera(mCamera);
	mGround->Render(0, 1);

//	mSphere->MaterialColor = vec3(0.0f, 0.0f, 1.0f);
//	mSphere->SetWorldScale(vec3(1.0f));
//	mSphere->SetWorldTranslation(vec3(0.0f, 0.0f, 0.0f));
//	mSphere->Render(0, 0);
//
//	float r = 10.0f;
//	mSphere->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
//	for( int i = 0; i < RSM_SAMPLE_COUNT; ++i )
//	{
//		float x, y, w;
//		x = r * randmoNumbers[3*i + 0];
//		y = r * randmoNumbers[3*i + 1];
//        w = randmoNumbers[3*i + 2];
//
//		mSphere->SetWorldScale(vec3(w));
//		mSphere->SetWorldTranslation(vec3(x, y, 0.0f));
//		mSphere->Render(0, 0);
//	}
}
//----------------------------------------------------------------------------
void RSMApp::Run()
{
	// Generate RSM-buffer.
	mRSMBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawSceneToRSMBuffer();
	mRSMBuffer->Disable();

	// Generate indirect lighting buffer.
    mGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    DrawScene();
    mGBuffer->Disable();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if( mShowMode == SM_DeferredLighting )
    {
        // Deferred lighting.
        mRSMDeferredLightingQuad->Render(0, 0);
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
	mRSMNormalTexture = 0;
	mRSMPositionTexture = 0;
	mRSMFluxTexture = 0;
	mRSMDepthTexture = 0;

	mGBuffer = 0;
    mPositionTexture = 0;
    mNormalTexture = 0;
    mColorTexture = 0;
    mDepthTexture = 0;
	mIndirectLightingTexture = 0;

	mRSMTempResultQuad = 0;
    mRSMDeferredLightingQuad = 0;
    
	mSphere = 0;
	mSamplingPatternTexture = 0;
}
//----------------------------------------------------------------------------
void RSMApp::OnKeyboard(unsigned char key, int x, int y)
{
	switch( key )
	{
	case '1':
		mShowMode = SM_RSMPosition;
        mRSMTempResultQuad->ShowMode = 1;
		mRSMTempResultQuad->TempTexture = mRSMPositionTexture;
		break;

	case '2':
		mShowMode = SM_RSMNormal;
        mRSMTempResultQuad->ShowMode = 2;
        mRSMTempResultQuad->TempTexture = mRSMNormalTexture;
		break;

	case '3':
		mShowMode = SM_RSMFlux;
		mRSMTempResultQuad->ShowMode = 3;
		mRSMTempResultQuad->TempTexture = mRSMFluxTexture;
		break;

	case '4':
		mShowMode = SM_RSMDepth;
        mRSMTempResultQuad->ShowMode = 4;
        mRSMTempResultQuad->TempTexture = mRSMDepthTexture;
		break;

	case '5':
		mShowMode = SM_IndirectLighting;
        mRSMTempResultQuad->ShowMode = 5;
        mRSMTempResultQuad->TempTexture = mIndirectLightingTexture;
        break;
            
    case '6':
        mShowMode = SM_Position;
        mRSMTempResultQuad->ShowMode = 6;
        mRSMTempResultQuad->TempTexture = mPositionTexture;
        break;
          
    case '7':
        mShowMode = SM_Normal;
        mRSMTempResultQuad->ShowMode = 7;
        mRSMTempResultQuad->TempTexture = mNormalTexture;
        break;
           
    case '8':
        mShowMode = SM_Color;
        mRSMTempResultQuad->ShowMode = 8;
        mRSMTempResultQuad->TempTexture = mColorTexture;
        break;

    case '9':
        mShowMode = SM_Depth;
        mRSMTempResultQuad->ShowMode = 9;
        mRSMTempResultQuad->TempTexture = mDepthTexture;
        break;
            
    case '0':
        mShowMode = SM_DeferredLighting;
        break;
            
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