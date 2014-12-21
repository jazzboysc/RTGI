#include "BidirectionalVoxelGIApp.h"
#include "InformationPanel.h"
#include <glfw3.h>

using namespace RTGI;
using namespace RTGI::GUIFramework;

//----------------------------------------------------------------------------
BidirectionalVoxelGIApp::BidirectionalVoxelGIApp(int width, int height)
{
	Width = width;
	Height = height;
	Title = "Bidirectional Voxel GI";
    mShowMode = SM_Final;
    mIsRotatingModel = false;
    mIsWireframe = false;
}
//----------------------------------------------------------------------------
BidirectionalVoxelGIApp::~BidirectionalVoxelGIApp()
{
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::Initialize(GPUDevice* device)
{
	float color = 0.5f;
    glClearColor(color, color, color, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create scene camera.
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width/(float)Height, 0.01f, 50.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 10.0f, 33.2f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    // Create light projector.
    mLightProjector = new Camera();
    mLightProjector->SetPerspectiveFrustum(85.0f, 1.0f, 0.01f, 50.0f);
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

	// Create material templates.
	Material* material = 0;

    ShaderProgramInfo voxelizationProgramInfo;
    voxelizationProgramInfo.VShaderFileName = "BidirectionalVoxelGI/vVoxelization.glsl";
    voxelizationProgramInfo.GShaderFileName = "BidirectionalVoxelGI/gVoxelization.glsl";
    voxelizationProgramInfo.FShaderFileName = "BidirectionalVoxelGI/fVoxelization.glsl";
    voxelizationProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                              ShaderType::ST_Geometry |
                                              ShaderType::ST_Fragment;
    Pass* passVoxelization = new Pass(voxelizationProgramInfo);

    ShaderProgramInfo shadowProgramInfo;
    shadowProgramInfo.VShaderFileName = "BidirectionalVoxelGI/vShadow.glsl";
    shadowProgramInfo.FShaderFileName = "BidirectionalVoxelGI/fShadow.glsl";
    shadowProgramInfo.TCShaderFileName = "BidirectionalVoxelGI/tcShadow.glsl";
    shadowProgramInfo.TEShaderFileName = "BidirectionalVoxelGI/teShadow.glsl";
    shadowProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                        ShaderType::ST_Fragment | 
                                        ShaderType::ST_TessellationControl |
                                        ShaderType::ST_TessellationEvaluation;
    Pass* passShadow = new Pass(shadowProgramInfo);

    ShaderProgramInfo gbufferProgramInfo;
    gbufferProgramInfo.VShaderFileName = "BidirectionalVoxelGI/vGBuffer.glsl";
    gbufferProgramInfo.FShaderFileName = "BidirectionalVoxelGI/fGBuffer.glsl";
    gbufferProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passGBuffer = new Pass(gbufferProgramInfo);

    ShaderProgramInfo rsmProgramInfo;
    rsmProgramInfo.VShaderFileName = "BidirectionalVoxelGI/vRSM.glsl";
    rsmProgramInfo.GShaderFileName = "BidirectionalVoxelGI/gRSM.glsl";
    rsmProgramInfo.FShaderFileName = "BidirectionalVoxelGI/fRSM.glsl";
    rsmProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                     ShaderType::ST_Geometry |
                                     ShaderType::ST_Fragment;
    Pass* passRSM = new Pass(rsmProgramInfo);

	Technique* techScene = new Technique();
    techScene->AddPass(passVoxelization);
    techScene->AddPass(passShadow);
    techScene->AddPass(passGBuffer);
    techScene->AddPass(passRSM);
	MaterialTemplate* mtScene = new MaterialTemplate();
    mtScene->AddTechnique(techScene);

    ShaderProgramInfo vplTempProgramInfo;
    vplTempProgramInfo.VShaderFileName = "BidirectionalVoxelGI/vTempResult.glsl";
    vplTempProgramInfo.FShaderFileName = "BidirectionalVoxelGI/fTempResult.glsl";
    vplTempProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                         ShaderType::ST_Fragment;
    Pass* passScreenQuad = new Pass(vplTempProgramInfo);

    Technique* techScreenQuad = new Technique();
    techScreenQuad->AddPass(passScreenQuad);
    MaterialTemplate* mtScreenQuad = new MaterialTemplate();
    mtScreenQuad->AddTechnique(techScreenQuad);

    // Create scene voxelizer.
    mVoxelizer = new Voxelizer();
    mVoxelizer->Initialize(mDevice, VOXEL_DIMENSION, VOXEL_LOCAL_GROUP_DIM, &mSceneBB);
    mVoxelBuffer = (StructuredBuffer*)mVoxelizer->GetGenericBufferByName(RTGI_VoxelBuffer_Name);

    // Create G-buffer renderer.
    mGBufferRenderer = new GBufferRenderer();
    mGBufferRenderer->CreateGBuffer(Width, Height, Texture::TF_RGBAF);
    mGBufferPositionTexture = (Texture2D*)mGBufferRenderer->GetFrameBufferTextureByName(RTGI_GBuffer_Position_Name);
    mGBufferNormalTexture = (Texture2D*)mGBufferRenderer->GetFrameBufferTextureByName(RTGI_GBuffer_Normal_Name);
    mGBufferAlbedoTexture = (Texture2D*)mGBufferRenderer->GetFrameBufferTextureByName(RTGI_GBuffer_Albedo_Name);

    // Create shadow map renderer.
    mShadowMapRenderer = new ShadowMapRenderer();
    mShadowMapRenderer->CreateShadowMap(1024, 1024, Texture::TF_RGBAF);
    mShadowMapTexture = (Texture2D*)mShadowMapRenderer->GetFrameBufferTexture(0);

    // Create RSM renderer.
    mRSMRenderer = new RSMRenderer();
    mRSMRenderer->CreateRSM(256, 256, RSM_FACE_COUNT, Texture::TF_RGBAF);
    mRSMPositionTextureArray = (Texture2DArray*)mRSMRenderer->GetFrameBufferTexture(0);
    mRSMNormalTextureArray = (Texture2DArray*)mRSMRenderer->GetFrameBufferTexture(1);
    mRSMFluxTextureArray = (Texture2DArray*)mRSMRenderer->GetFrameBufferTexture(2);

    // Create VPL generator.
    mVPLGenerator = new VPLGenerator();
    mVPLGenerator->SetRSMRenderer(mRSMRenderer);
    mVPLGenerator->Initialize(mDevice, VPL_SAMPLE_COUNT);

    // Create direct lighting renderer.
    mDirectLightingRenderer = new DirectLightingRenderer();
    mDirectLightingRenderer->SetInputs(mGBufferRenderer, mShadowMapRenderer);
    mDirectLightingRenderer->Initialize(mDevice, Width, Height, Texture::TF_RGBAF, mLightProjector);
    mDirectLightingTexture = (Texture2D*)mDirectLightingRenderer->GetFrameBufferTexture(0);

    // Create indirect lighting renderer.
    mIndirectLightingRenderer = new IndirectLightingRenderer();
    mIndirectLightingRenderer->SetInputs(mGBufferRenderer, mVPLGenerator);
    mIndirectLightingRenderer->Initialize(mDevice, Width, Height, Texture::TF_RGBAF, VPL_SAMPLE_COUNT);
    mIndirectLightingTexture = (Texture2D*)mIndirectLightingRenderer->GetFrameBufferTexture(0);

    // Create GPU timer.
    mTimer = new GPUTimer();
    mTimer->CreateDeviceResource();
    mVoxelizer->SetTimer(mTimer);
    mShadowMapRenderer->SetTimer(mTimer);
    mGBufferRenderer->SetTimer(mTimer);
    mRSMRenderer->SetTimer(mTimer);
    mVPLGenerator->SetTimer(mTimer);
    mDirectLightingRenderer->SetTimer(mTimer);
    mIndirectLightingRenderer->SetTimer(mTimer);

	// Create scene.
    mSceneObjects = new RenderSet();
    mVoxelizer->SetRenderSet(mSceneObjects);
    mShadowMapRenderer->SetRenderSet(mSceneObjects);
    mGBufferRenderer->SetRenderSet(mSceneObjects);
    mRSMRenderer->SetRenderSet(mSceneObjects);

	mat4 rotM;
    material = new Material(mtScene);
	mModel = new SceneMesh(material, mMainCamera);
	mModel->LoadFromFile("dragon_s.ply");
    mat4 scale = glm::scale(mat4(), vec3(60.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource(mDevice);
	mModel->SetWorldTranslation(vec3(0.0f, 4.0f, 3.0f));
	mModel->MaterialColor = vec3(1.8f, 1.8f, 1.8f);
    mModel->LightProjector = mLightProjector;
    mModel->SceneBB = &mSceneBB;
    mSceneBB.Merge(mModel->GetWorldSpaceBB());

    material = new Material(mtScene);
	mGround = new SceneMesh(material, mMainCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource(mDevice);
    mGround->MaterialColor = vec3(1.5f, 1.5f, 1.5f);
    mGround->LightProjector = mLightProjector;
    mSceneObjects->AddRenderObject(mGround);
    mGround->SceneBB = &mSceneBB;
    mSceneBB.Merge(mGround->GetWorldSpaceBB());

    material = new Material(mtScene);
	mCeiling = new SceneMesh(material, mMainCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource(mDevice);
	rotM = rotate(mat4(), radians(180.0f), vec3(1, 0, 0));
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(1.5f, 1.5f, 1.5f);
    mCeiling->LightProjector = mLightProjector;
    mSceneObjects->AddRenderObject(mCeiling);
    mCeiling->SceneBB = &mSceneBB;
    mSceneBB.Merge(mCeiling->GetWorldSpaceBB());

    material = new Material(mtScene);
	mBackWall = new SceneMesh(material, mMainCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(1.5f, 1.5f, 1.5f);
    mBackWall->LightProjector = mLightProjector;
    mSceneObjects->AddRenderObject(mBackWall);
    mBackWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mBackWall->GetWorldSpaceBB());

    material = new Material(mtScene);
	mLeftWall = new SceneMesh(material, mMainCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(-90.0f), vec3(0, 0, 1));
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.2f, 0.2f);
    mLeftWall->LightProjector = mLightProjector;
    mSceneObjects->AddRenderObject(mLeftWall);
    mLeftWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mLeftWall->GetWorldSpaceBB());

    material = new Material(mtScene);
	mRightWall = new SceneMesh(material, mMainCamera);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(0, 0, 1));
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.2f, 1.0f, 0.2f);
    mRightWall->LightProjector = mLightProjector;
    mSceneObjects->AddRenderObject(mRightWall);
    mRightWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mRightWall->GetWorldSpaceBB());

    mSceneObjects->AddRenderObject(mModel);

    // Create screen quads.
    material = new Material(mtScreenQuad);
    mTempScreenQuad = new TempScreenQuad(material);
    mTempScreenQuad->LoadFromFile("screenquad.ply");
    mTempScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mTempScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mTempScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mTempScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mTempScreenQuad->CreateDeviceResource(mDevice);
    mTempScreenQuad->ShowMode = 2;
    mTempScreenQuad->TempTexture = mIndirectLightingTexture;
    mTempScreenQuad->TempTexture2 = mDirectLightingTexture;
    mTempScreenQuad->TempTextureArray = mRSMFluxTextureArray;
    mTempScreenQuad->SceneBB = &mSceneBB;
    mTempScreenQuad->VoxelBuffer = mVoxelBuffer;
    mTempScreenQuad->VoxelGridDim = VOXEL_DIMENSION;

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
    InformationPanel::GetInstance()->AddTimingLabel("Scene G-buffer Pass", 16, infoStartY);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddTimingLabel("RSM Pass", 16, infoStartY);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddTimingLabel("VPL Creation Pass", 16, infoStartY);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddTimingLabel("Direct Lighting Pass", 16, infoStartY);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddTimingLabel("Indirect Lighting Pass", 16, infoStartY);
    infoStartY = 20;
    InformationPanel::GetInstance()->AddRadioButton("Voxel Buffer", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("Scene Shadow Map", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("G-Buffer Position", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("G-Buffer Normal", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("G-Buffer Albedo", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("RSM Position", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("RSM Normal", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("RSM Flux", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("Direct Lighting", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("Indirect Lighting", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("Final Result", 16, infoStartY, 60, 20, true);
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::FrameFunc()
{
    static float angle = 0.0f;
    if( mIsRotatingModel )
    {
        angle += 1.0f;
        mat4 rot;
        rot = rotate(mat4(), radians(angle), vec3(0, 1, 0));
        vec3 trans = mModel->GetWorldTranslation();
        mModel->SetWorldTransform(rot);
        mModel->SetWorldTranslation(trans);
    }

    if( mIsWireframe )
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    InformationPanel^ infoPanel = InformationPanel::GetInstance();
    static double workLoad = 0.0;

    // Scene voxelization pass.
    mVoxelizer->Render(0, SMP_Voxelization);
    workLoad = mVoxelizer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Scene Voxelization Pass", workLoad);
    glViewport(0, 0, Width, Height);

    // Scene shadow pass.
    mShadowMapRenderer->Render(0, SMP_ShadowMap, mLightProjector);
    workLoad = mShadowMapRenderer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Scene Shadow Pass", workLoad);

    // Scene G-buffer pass.
    mGBufferRenderer->Render(0, SMP_GBuffer, mMainCamera);
    workLoad = mGBufferRenderer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Scene G-buffer Pass", workLoad);

    // Scene light RSM pass.
    mRSMRenderer->Render(0, SMP_RSM, 0);
    workLoad = mRSMRenderer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("RSM Pass", workLoad);

    // Sample RSM pass (VPL generation).
    mVPLGenerator->Run();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("VPL Creation Pass", workLoad);

    // Deferred direct illumination pass.
    mDirectLightingRenderer->Render();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Direct Lighting Pass", workLoad);

    // Deferred indirect illumination pass.
    mIndirectLightingRenderer->Render();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Indirect Lighting Pass", workLoad);

    // Show rendering result.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mTempScreenQuad->Render(0, 0);
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::Terminate()
{
	// Release all resources.
    delete mLightProjector;

    mVoxelizer = 0;

    mGBufferRenderer = 0;
    mGBufferPositionTexture = 0;
    mGBufferNormalTexture = 0;
    mGBufferAlbedoTexture = 0;

    mShadowMapRenderer = 0;
    mShadowMapTexture = 0;

    mDirectLightingRenderer = 0;
    mDirectLightingTexture = 0;

    mIndirectLightingRenderer = 0;
    mIndirectLightingTexture = 0;

    mRSMRenderer = 0;
    mRSMPositionTextureArray = 0;
    mRSMNormalTextureArray = 0;
    mRSMFluxTextureArray = 0;

    mVPLGenerator = 0;

    mTempScreenQuad = 0;

	mGround = 0;
	mCeiling = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;
    mSceneObjects = 0;

    mTimer = 0;
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::ProcessInput()
{
	bool isShift = glfwGetKey(Window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(Window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
	bool isCap = isShift ^ (glfwGetKey(Window, GLFW_KEY_CAPS_LOCK) == GLFW_PRESS);
	if (isCap)
	{
		// Uppercase
		if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_PRESS)
		{
			mTempScreenQuad->TextureArrayIndex = 0;
		}
		if (glfwGetKey(Window, GLFW_KEY_Y) == GLFW_PRESS)
		{
			mTempScreenQuad->TextureArrayIndex = 2;
		}
		if (glfwGetKey(Window, GLFW_KEY_Z) == GLFW_PRESS)
		{
		}
	}
	else
	{
		// Lowercase
		if (glfwGetKey(Window, GLFW_KEY_X) == GLFW_PRESS)
		{
			mTempScreenQuad->TextureArrayIndex = 1;
		}
		if (glfwGetKey(Window, GLFW_KEY_Y) == GLFW_PRESS)
		{
			mTempScreenQuad->TextureArrayIndex = 3;
		}
		if (glfwGetKey(Window, GLFW_KEY_Z) == GLFW_PRESS)
		{
			mTempScreenQuad->TextureArrayIndex = 4;
		}
	}

	if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS)
	{
		mIsRotatingModel = !mIsRotatingModel;
	}
	if (glfwGetKey(Window, GLFW_KEY_T) == GLFW_PRESS)
	{
		mIsWireframe = !mIsWireframe;
	}
	if (glfwGetKey(Window, GLFW_KEY_EQUAL) == GLFW_PRESS)
	{
		if (mShowMode == SM_VPLShadow)
		{
			mTempScreenQuad->TextureArrayIndex++;
			mTempScreenQuad->TextureArrayIndex = mTempScreenQuad->TextureArrayIndex % VPL_SAMPLE_COUNT;
		}
	}
	if (glfwGetKey(Window, GLFW_KEY_MINUS) == GLFW_PRESS)
	{
		if (mShowMode == SM_VPLShadow)
		{
			mTempScreenQuad->TextureArrayIndex--;
			mTempScreenQuad->TextureArrayIndex = mTempScreenQuad->TextureArrayIndex % VPL_SAMPLE_COUNT;
		}
	}
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::OnRadioButtonClick(System::Object^  sender, System::EventArgs^  e)
{
    RadioButton^ radioButton = (RadioButton^)sender;
    if( !mTempScreenQuad )
    {
        return;
    }

    if( radioButton->Name == "Voxel Buffer" )
    {
        mShowMode = SM_VoxelBuffer;
        mTempScreenQuad->ShowMode = 4;
        mTempScreenQuad->TempTexture = mGBufferPositionTexture;
    }

    if( radioButton->Name == "Scene Shadow Map" )
    {
        mShowMode = SM_Shadow;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mShadowMapTexture;
    }

    if( radioButton->Name == "G-Buffer Position" )
    {
        mShowMode = SM_GBufferPosition;
        mTempScreenQuad->ShowMode = 3;
        mTempScreenQuad->TempTexture = mGBufferPositionTexture;
    }

    if( radioButton->Name == "G-Buffer Normal" )
    {
        mShowMode = SM_GBufferNormal;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mGBufferNormalTexture;
    }

    if( radioButton->Name == "G-Buffer Albedo" )
    {
        mShowMode = SM_GBufferAlbedo;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mGBufferAlbedoTexture;
    }

    if( radioButton->Name == "RSM Position" )
    {
        mShowMode = SM_RSMPosition;
        mTempScreenQuad->ShowMode = 1;
        mTempScreenQuad->TempTextureArray = mRSMPositionTextureArray;
    }

    if( radioButton->Name == "RSM Normal" )
    {
        mShowMode = SM_RSMNormal;
        mTempScreenQuad->ShowMode = 1;
        mTempScreenQuad->TempTextureArray = mRSMNormalTextureArray;
    }

    if( radioButton->Name == "RSM Flux" )
    {
        mShowMode = SM_RSMFlux;
        mTempScreenQuad->ShowMode = 1;
        mTempScreenQuad->TempTextureArray = mRSMFluxTextureArray;
    }

    if( radioButton->Name == "Direct Lighting" )
    {
        mShowMode = SM_DirectLighting;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mDirectLightingTexture;
    }

    if( radioButton->Name == "Indirect Lighting" )
    {
        mShowMode = SM_IndirectLighting;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mIndirectLightingTexture;
    }

    if( radioButton->Name == "Final Result" )
    {
        mShowMode = SM_Final;
        mTempScreenQuad->ShowMode = 2;
        mTempScreenQuad->TempTexture = mIndirectLightingTexture;
        mTempScreenQuad->TempTexture2 = mDirectLightingTexture;
    }
}
//----------------------------------------------------------------------------