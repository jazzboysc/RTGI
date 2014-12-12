#include "SSDOApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
SSDOApp::SSDOApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("SSDO demo")
{
	mShowMode = SM_FilteredSSDO;
}
//----------------------------------------------------------------------------
SSDOApp::~SSDOApp()
{
}
//----------------------------------------------------------------------------
void SSDOApp::Initialize(GPUDevice* device)
{
    Application::Initialize(device);

	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.5f;
	glClearColor(color, color, color, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create camera.
	mCamera = new Camera;
	mCamera->SetPerspectiveFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 100.0f);
	mCamera->SetLookAt(vec3(0.0f, 9.0f, 26.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));

	// Create light.
	mLight = new Light;
	mLight->SetLocation(vec3(0.0f, 10.0f, 5.0f));

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo gbufferProgramInfo;
    gbufferProgramInfo.VShaderFileName = "SSDO/vGBuffer.glsl";
    gbufferProgramInfo.FShaderFileName = "SSDO/fGBuffer.glsl";
    gbufferProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                         ShaderType::ST_Fragment;
    Pass* passGBuffer = new Pass(gbufferProgramInfo);
	Technique* techGBuffer = new Technique();
	techGBuffer->AddPass(passGBuffer);
	MaterialTemplate* mtGBuffer = new MaterialTemplate();
	mtGBuffer->AddTechnique(techGBuffer);

    ShaderProgramInfo directLightingProgramInfo;
    directLightingProgramInfo.VShaderFileName = "SSDO/vDirectLighting.glsl";
    directLightingProgramInfo.FShaderFileName = "SSDO/fDirectLighting.glsl";
    directLightingProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                                ShaderType::ST_Fragment;
    Pass* passDirectLighting = new Pass(directLightingProgramInfo);
	Technique* techDirectLighting = new Technique();
	techDirectLighting->AddPass(passDirectLighting);
	MaterialTemplate* mtDirectLighting = new MaterialTemplate();
	mtDirectLighting->AddTechnique(techDirectLighting);

    ShaderProgramInfo ssdoProgramInfo;
    ssdoProgramInfo.VShaderFileName = "SSDO/vSSDO.glsl";
    ssdoProgramInfo.FShaderFileName = "SSDO/fSSDO.glsl";
    ssdoProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                      ShaderType::ST_Fragment;
    Pass* passSSDO = new Pass(ssdoProgramInfo);
	Technique* techSSDO = new Technique();
	techSSDO->AddPass(passSSDO);
	MaterialTemplate* mtSSDO = new MaterialTemplate();
	mtSSDO->AddTechnique(techSSDO);

    ShaderProgramInfo ssdoFilterProgramInfo;
    ssdoFilterProgramInfo.VShaderFileName = "SSDO/vSSDOFilter.glsl";
    ssdoFilterProgramInfo.FShaderFileName = "SSDO/fSSDOFilter.glsl";
    ssdoFilterProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                            ShaderType::ST_Fragment;
    Pass* passSSDOFilter = new Pass(ssdoFilterProgramInfo);
	Technique* techSSDOFilter = new Technique();
	techSSDOFilter->AddPass(passSSDOFilter);
	MaterialTemplate* mtSSDOFilter = new MaterialTemplate();
	mtSSDOFilter->AddTechnique(techSSDOFilter);

    ShaderProgramInfo ssdoTempProgramInfo;
    ssdoTempProgramInfo.VShaderFileName = "SSDO/vSSDOTemp.glsl";
    ssdoTempProgramInfo.FShaderFileName = "SSDO/fSSDOTemp.glsl";
    ssdoTempProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                          ShaderType::ST_Fragment;
    Pass* passSSDOTemp = new Pass(ssdoTempProgramInfo);
	Technique* techSSDOTemp = new Technique();
	techSSDOTemp->AddPass(passSSDOTemp);
	MaterialTemplate* mtSSDOTemp = new MaterialTemplate();
	mtSSDOTemp->AddTechnique(techSSDOTemp);

	// Create environment map texture.
	mEnvTexture = new Texture2D();
	mEnvTexture->LoadPFMFromFile("KitchenMediumBlurred.pfm");

	// Create LD random texture.
	mRandomTexture = new Texture2D();
	mRandomTexture->CreateLDRandomTextureRGBF(16, 2);

	// Create MRT textures.
	mPositionTexture = new Texture2D();
	mPositionTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
	mNormalTexture = new Texture2D();
	mNormalTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
	mColorTexture = new Texture2D();
	mColorTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
	mDepthTexture = new Texture2D();
	mDepthTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_Depth);

	// Create G-buffer.
	Texture* colorTextures[3] = {mPositionTexture, mNormalTexture, mColorTexture};
	mGBuffer = new FrameBuffer();
	mGBuffer->SetRenderTargets(3, colorTextures, mDepthTexture);

	// Create direct lighting render target.
	mDirectLightingTexture = new Texture2D();
	mDirectLightingTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);

	// Create direct lighting framebuffer.
	Texture* directLightingTexture[1] = {mDirectLightingTexture};
	mDirectLightingBuffer = new FrameBuffer();
	mDirectLightingBuffer->SetRenderTargets(1, directLightingTexture, mDepthTexture);

	// Create SSDO render target.
	mSSDOTexture = new Texture2D();
	mSSDOTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);

	// Create SSDO framebuffer.
	Texture* ssdoTexture[1] = {mSSDOTexture};
	mSSDOBuffer = new FrameBuffer();
	mSSDOBuffer->SetRenderTargets(1, ssdoTexture, mDepthTexture);

    vec2 tcoord00(0.0f, 0.0f);
    vec2 tcoord01(0.0f, 1.0f);
    vec2 tcoord10(1.0f, 0.0f);
    vec2 tcoord11(1.0f, 1.0f);
    
	// Create direct lighting screen quad.
	material = new Material(mtDirectLighting);
	mDirectLightingQuad = new DirectLightingQuad(material, mLight);
	mDirectLightingQuad->LoadFromFile("screenquad.ply");
	mDirectLightingQuad->SetTCoord(0, tcoord00);
	mDirectLightingQuad->SetTCoord(1, tcoord10);
	mDirectLightingQuad->SetTCoord(2, tcoord11);
	mDirectLightingQuad->SetTCoord(3, tcoord01);
	mDirectLightingQuad->CreateDeviceResource(mDevice);
	mDirectLightingQuad->PositionTexture = mPositionTexture;
	mDirectLightingQuad->NormalTexture = mNormalTexture;
	mDirectLightingQuad->ReflectanceTexture = mColorTexture;

	// Create SSDO screen quad.
	material = new Material(mtSSDO);
	mSSDOQuad = new SSDOScreenQuad(material, mCamera);
	mSSDOQuad->LoadFromFile("screenquad.ply");
	mSSDOQuad->SetTCoord(0, tcoord00);
	mSSDOQuad->SetTCoord(1, tcoord10);
	mSSDOQuad->SetTCoord(2, tcoord11);
	mSSDOQuad->SetTCoord(3, tcoord01);
	mSSDOQuad->CreateDeviceResource(mDevice);
	mSSDOQuad->PositionTexture = mPositionTexture;
	mSSDOQuad->NormalTexture = mNormalTexture;
	mSSDOQuad->ColorTexture = mColorTexture;
	mSSDOQuad->DirectLightingTexture = mDirectLightingTexture;
	mSSDOQuad->RandomTexture = mRandomTexture;
	mSSDOQuad->EnvMapTexture = mEnvTexture;
	mSSDOQuad->SampleRadius = 1.0f;
	mSSDOQuad->Strength = 1.0f;
	mSSDOQuad->Singularity = 5.0f;
	mSSDOQuad->DepthBias = 0.1f;
	mSSDOQuad->BounceStrength = 107.0f;
	mSSDOQuad->BounceSingularity = 500.0f;
	mSSDOQuad->SampleCount = 16;
	mSSDOQuad->PatternSize = 2;

	// Create SSDO filter screen quad.
	material = new Material(mtSSDOFilter);
	mSSDOFilterQuad = new SSDOFilterScreenQuad(material);
	mSSDOFilterQuad->LoadFromFile("screenquad.ply");
	mSSDOFilterQuad->SetTCoord(0, tcoord00);
	mSSDOFilterQuad->SetTCoord(1, tcoord10);
	mSSDOFilterQuad->SetTCoord(2, tcoord11);
	mSSDOFilterQuad->SetTCoord(3, tcoord01);
	mSSDOFilterQuad->CreateDeviceResource(mDevice);
	mSSDOFilterQuad->SSDOTexture = mSSDOTexture;
	mSSDOFilterQuad->PositionTexture = mPositionTexture;
	mSSDOFilterQuad->NormalTexture = mNormalTexture;
	mSSDOFilterQuad->ColorTexture = mColorTexture;
	mSSDOFilterQuad->DirectLightingTexture = mDirectLightingTexture;
	mSSDOFilterQuad->PositionThreshold = 5.5f;
	mSSDOFilterQuad->NormalThreshold = 0.3f;
	mSSDOFilterQuad->MaxRadiance = 4.5f;
	mSSDOFilterQuad->KernelSize = 2;

	// Create SSDO temp result screen quad.
	material = new Material(mtSSDOTemp);
	mSSDOTempResultQuad = new SSDOTempScreenQuad(material);
	mSSDOTempResultQuad->LoadFromFile("screenquad.ply");
	mSSDOTempResultQuad->SetTCoord(0, tcoord00);
	mSSDOTempResultQuad->SetTCoord(1, tcoord10);
	mSSDOTempResultQuad->SetTCoord(2, tcoord11);
	mSSDOTempResultQuad->SetTCoord(3, tcoord01);
	mSSDOTempResultQuad->CreateDeviceResource(mDevice);
	mSSDOTempResultQuad->TempTexture = mSSDOTexture;

	// Create scene.
	material = new Material(mtGBuffer);
	mModel1 = new SSDOTriMesh(material, mCamera);
	mModel1->LoadFromFile("dragon_s.ply");
	mModel1->GenerateNormals();
	mModel1->CreateDeviceResource(mDevice);
	mat4 rot = rotate(mat4(), radians(30.0f), vec3(0, 1, 0));
	mModel1->SetWorldTransform(rot);
	mModel1->SetWorldTranslation(vec3(-5.0f, 1.2f, -10.0f));
    mModel1->SetWorldScale(vec3(75.0f));
	mModel1->MaterialColor = vec3(1.5f, 1.5f, 1.5f);

	material = new Material(mtGBuffer);
	mModel2 = new SSDOTriMesh(material, mCamera);
	mModel2->LoadFromFile("happy_s.ply");
	mModel2->GenerateNormals();
	mModel2->CreateDeviceResource(mDevice);
    rot = rotate(mat4(), radians(0.0f), vec3(0, 1, 0));
	mModel2->SetWorldTransform(rot);
	mModel2->SetWorldTranslation(vec3(5.0f, 0.0f, 5.0f));
    mModel2->SetWorldScale(vec3(60.0f));
	mModel2->MaterialColor = vec3(1.5f, 1.5f, 1.5f);

	material = new Material(mtGBuffer);
	mGround = new SSDOTriMesh(material, mCamera);
	mGround->LoadFromFile("ground.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource(mDevice);
	mGround->SetWorldTranslation(vec3(0.0f, -6.0f, 0.0f));
	mGround->MaterialColor = vec3(1.0f, 1.0f, 1.0f);

	float maxRadiusX = 30.0f;
	float maxRadiusZ = 60.0f;
	for( int i = 0; i < CubeCount; ++i )
	{
		material = new Material(mtGBuffer);
		mCubes[i] = new SSDOTriMesh(material, mCamera);
		mCubes[i]->LoadFromFile("cube.ply");
		mCubes[i]->GenerateNormals();
		mCubes[i]->CreateDeviceResource(mDevice);

Do:
		vec3 position;
		position.x = (float)SymmetricRandom() * maxRadiusX;
		position.y = -5.0f;
		position.z = (float)UniformRandom() * -maxRadiusZ;

		for( int j = 0; j < i; ++j )
		{
			vec3 direction = mCubes[j]->GetWorldTranslation() - position;
			float distance = length(direction);
			if( distance < 3.0 )
			{
				goto Do;
			}
		}

		float a =(float)SymmetricRandom() * 90.0f;
        mat4 rot = rotate(mat4(), radians(a), vec3(0, 1, 0));
		mCubes[i]->SetWorldTransform(rot);
		mCubes[i]->SetWorldTranslation(position);

		vec3 color;
		color.x = (float)UniformRandom();
		color.y = (float)UniformRandom();
		color.z = (float)UniformRandom();
		mCubes[i]->MaterialColor = color;
	}
}
//----------------------------------------------------------------------------
void SSDOApp::DrawScene()
{
	mModel1->Render(0, 0);
	mModel2->Render(0, 0);
	mGround->Render(0, 0);

	for( int i = 0; i < CubeCount; ++i )
	{
		mCubes[i]->Render(0, 0);
	}
}
//----------------------------------------------------------------------------
void SSDOApp::Run()
{
	//static DWORD time = 0;
	//DWORD curTime = ::GetTickCount();
	//if( curTime - time >= 10 )
	//{
	//	time = curTime;
	//}
	//else
	//{
	//	return;
	//}

	// Draw scene to G-buffer.
	mGBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawScene();
	mGBuffer->Disable();

	// Deferred lighting.
	mDirectLightingBuffer->Enable();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mDirectLightingQuad->Render(0, 0);
	mDirectLightingBuffer->Disable();

	// Generate SSDO buffer.
	mSSDOBuffer->Enable();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	mSSDOQuad->Render(0, 0);
	mSSDOBuffer->Disable();

	// Draw final image.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	switch (mShowMode)
	{
	case RTGI::SSDOApp::SM_Position:
		mSSDOTempResultQuad->Render(0, 0);
		break;
	case RTGI::SSDOApp::SM_Normal:
		mSSDOTempResultQuad->Render(0, 0);
		break;
	case RTGI::SSDOApp::SM_Color:
		mSSDOTempResultQuad->Render(0, 0);
		break;
	case RTGI::SSDOApp::SM_DirectLighting:
		mSSDOTempResultQuad->Render(0, 0);
		break;
	case RTGI::SSDOApp::SM_Random:
		mSSDOTempResultQuad->Render(0, 0);
		break;
	case RTGI::SSDOApp::SM_SSDO:
		mSSDOTempResultQuad->Render(0, 0);
		break;
	case RTGI::SSDOApp::SM_FilteredSSDO:
		mSSDOFilterQuad->Render(0, 0);
		break;
	default:
		break;
	}

	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void SSDOApp::Terminate()
{
	// Release all resources.

	delete mCamera;

	mLight = 0;
	mModel1 = 0;
	mModel2 = 0;
	mGround = 0;
	for( int i = 0; i < CubeCount; ++i )
	{
		mCubes[i] = 0;
	}

	mGBuffer = 0;
	mNormalTexture = 0;
	mPositionTexture = 0;
	mColorTexture = 0;
	mDepthTexture = 0;

	mEnvTexture = 0;

	mDirectLightingBuffer = 0;
	mDirectLightingTexture = 0;

	mSSDOBuffer = 0;
	mSSDOTexture = 0;
	mRandomTexture = 0;

	mSSDOQuad = 0;
	mSSDOFilterQuad = 0;
	mSSDOTempResultQuad = 0;
	mDirectLightingQuad = 0;
}
//----------------------------------------------------------------------------
void SSDOApp::OnKeyboard(unsigned char key, int x, int y)
{
	switch( key )
	{
	case '1':
		mShowMode = SM_FilteredSSDO;
		break;

	case '2':
		mSSDOTempResultQuad->TempTexture = mNormalTexture;
		mShowMode = SM_Normal;
		break;

	case '3':
		mSSDOTempResultQuad->TempTexture = mColorTexture;
		mShowMode = SM_Color;
		break;

	case '4':
		mSSDOTempResultQuad->TempTexture = mDirectLightingTexture;
		mShowMode = SM_DirectLighting;
		break;

	case '5':
		mSSDOTempResultQuad->TempTexture = mRandomTexture;
		mShowMode = SM_Random;

	default:
		break;
	}
}
//----------------------------------------------------------------------------
void SSDOApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void SSDOApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void SSDOApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void SSDOApp::OnReshape(int x, int y)
{
	mWidth = x;
	mHeight = y;
}
//----------------------------------------------------------------------------