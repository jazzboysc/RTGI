#include "BidirectionalVoxelGIApp.h"
#include "RNG.h"
#include "InformationPanel.h"

using namespace RTGI;
using namespace RTGI::GUIFramework;

//----------------------------------------------------------------------------
BidirectionalVoxelGIApp::BidirectionalVoxelGIApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("Bidirectional Voxel GI")
{
    mShowMode = SM_Final;
    mIsRotatingModel = false;
    mIsWireframe = false;
}
//----------------------------------------------------------------------------
BidirectionalVoxelGIApp::~BidirectionalVoxelGIApp()
{
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::Initialize()
{
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.5f;
    glClearColor(color, color, color, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create scene camera.
	mCamera = new Camera();
	mCamera->SetPerspectiveFrustum(45.0f, (float)mWidth/(float)mHeight, 0.01f, 50.0f);
	mCamera->SetLookAt(vec3(0.0f, 10.0f, 33.2f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    // Create light projector.
    mLightProjector = new Camera();
    mLightProjector->SetPerspectiveFrustum(85.0f, 1.0f, 0.01f, 50.0f);
    mLightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));

	// Create material templates.
	Material* material = 0;

    ShaderProgramInfo shadowProgramInfo;
    shadowProgramInfo.VShaderFileName = "vShadow.glsl";
    shadowProgramInfo.FShaderFileName = "fShadow.glsl";
    shadowProgramInfo.TCShaderFileName = "tcShadow.glsl";
    shadowProgramInfo.TEShaderFileName = "teShadow.glsl";
    shadowProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                        ShaderType::ST_Fragment | 
                                        ShaderType::ST_TessellationControl |
                                        ShaderType::ST_TessellationEvaluation;
    Pass* passShadow = new Pass(shadowProgramInfo);

    ShaderProgramInfo gbufferProgramInfo;
    gbufferProgramInfo.VShaderFileName = "vGBuffer.glsl";
    gbufferProgramInfo.FShaderFileName = "fGBuffer.glsl";
    gbufferProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passGBuffer = new Pass(gbufferProgramInfo);

    ShaderProgramInfo rsmProgramInfo;
    rsmProgramInfo.VShaderFileName = "vRSM.glsl";
    rsmProgramInfo.GShaderFileName = "gRSM.glsl";
    rsmProgramInfo.FShaderFileName = "fRSM.glsl";
    rsmProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                     ShaderType::ST_Geometry |
                                     ShaderType::ST_Fragment;
    Pass* passRSM = new Pass(rsmProgramInfo);

	Technique* techVPL = new Technique();
    techVPL->AddPass(passShadow);
    techVPL->AddPass(passGBuffer);
    techVPL->AddPass(passRSM);
	MaterialTemplate* mtVPL = new MaterialTemplate();
	mtVPL->AddTechnique(techVPL);

    ShaderProgramInfo vplTempProgramInfo;
    vplTempProgramInfo.VShaderFileName = "vVPLTemp.glsl";
    vplTempProgramInfo.FShaderFileName = "fVPLTemp.glsl";
    vplTempProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                         ShaderType::ST_Fragment;
    Pass* passScreenQuad = new Pass(vplTempProgramInfo);

    Technique* techScreenQuad = new Technique();
    techScreenQuad->AddPass(passScreenQuad);
    MaterialTemplate* mtScreenQuad = new MaterialTemplate();
    mtScreenQuad->AddTechnique(techScreenQuad);

    // Create G-buffer renderer.
    mGBufferRenderer = new GBufferRenderer();
    mGBufferRenderer->CreateGBuffer(mWidth, mHeight, Texture::TF_RGBAF);
    mGBufferPositionTexture = (Texture2D*)(BufferBase*)mGBufferRenderer->GetFrameBufferTarget(0)->OutputBuffer;
    mGBufferNormalTexture = (Texture2D*)(BufferBase*)mGBufferRenderer->GetFrameBufferTarget(1)->OutputBuffer;
    mGBufferAlbedoTexture = (Texture2D*)(BufferBase*)mGBufferRenderer->GetFrameBufferTarget(2)->OutputBuffer;

    // Create shadow map renderer.
    mShadowMapRenderer = new ShadowMapRenderer();
    mShadowMapRenderer->CreateShadowMap(1024, 1024, Texture::TF_RGBAF);
    mShadowMapTexture = (Texture2D*)(BufferBase*)mShadowMapRenderer->GetFrameBufferTarget(0)->OutputBuffer;

    // Create RSM renderer.
    mRSMRenderer = new RSMRenderer();
    mRSMRenderer->CreateRSM(256, 256, RSM_FACE_COUNT, Texture::TF_RGBAF);
    mRSMPositionTextureArray = (Texture2DArray*)(BufferBase*)mRSMRenderer->GetFrameBufferTarget(0)->OutputBuffer;
    mRSMNormalTextureArray = (Texture2DArray*)(BufferBase*)mRSMRenderer->GetFrameBufferTarget(1)->OutputBuffer;
    mRSMFluxTextureArray = (Texture2DArray*)(BufferBase*)mRSMRenderer->GetFrameBufferTarget(2)->OutputBuffer;

    // Create VPL generator.
    mVPLGenerator = new VPLGenerator();
    mVPLGenerator->SetRSMRenderer(mRSMRenderer);
    mVPLGenerator->Initialize();

    // Create direct lighting renderer.
    mDirectLightingRenderer = new DirectLightingRenderer();
    mDirectLightingRenderer->SetInputs(mGBufferRenderer, mShadowMapRenderer);
    mDirectLightingRenderer->Initialize(mWidth, mHeight, Texture::TF_RGBAF, mLightProjector);
    mDirectLightingTexture = (Texture2D*)(BufferBase*)mDirectLightingRenderer->GetFrameBufferTarget(0)->OutputBuffer;

    // Create indirect lighting renderer.
    mIndirectLightingRenderer = new IndirectLightingRenderer();
    mIndirectLightingRenderer->SetInputs(mGBufferRenderer, mVPLGenerator);
    mIndirectLightingRenderer->Initialize(mWidth, mHeight, Texture::TF_RGBAF, VPL_SAMPLE_COUNT);
    mIndirectLightingTexture = (Texture2D*)(BufferBase*)mIndirectLightingRenderer->GetFrameBufferTarget(0)->OutputBuffer;

    // Create GPU timer.
    mTimer = new GPUTimer();
    mTimer->CreateDeviceResource();
    mShadowMapRenderer->SetTimer(mTimer);
    mGBufferRenderer->SetTimer(mTimer);
    mRSMRenderer->SetTimer(mTimer);
    mVPLGenerator->SetTimer(mTimer);
    mDirectLightingRenderer->SetTimer(mTimer);
    mIndirectLightingRenderer->SetTimer(mTimer);

	// Create scene.
    mSceneObjects = new RenderSet();
    mShadowMapRenderer->SetRenderSet(mSceneObjects);
    mGBufferRenderer->SetRenderSet(mSceneObjects);
    mRSMRenderer->SetRenderSet(mSceneObjects);

	mat4 rotM;
	material = new Material(mtVPL);
	mModel = new SceneMesh(material, mCamera);
	mModel->LoadFromFile("dragon_s.ply");
    mat4 scale = Scale(vec3(60.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(0.0f, 4.0f, 3.0f));
	mModel->MaterialColor = vec3(1.8f, 1.8f, 1.8f);
    mModel->LightProjector = mLightProjector;

    mSceneObjects->AddRenderObject(mModel);

    material = new Material(mtVPL);
	mGround = new SceneMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(1.2f, 1.2f, 1.2f);
    mGround->LightProjector = mLightProjector;
    mSceneObjects->AddRenderObject(mGround);

    material = new Material(mtVPL);
	mCeiling = new SceneMesh(material, mCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(1.2f, 1.2f, 1.2f);
    mCeiling->LightProjector = mLightProjector;
    mSceneObjects->AddRenderObject(mCeiling);

    material = new Material(mtVPL);
	mBackWall = new SceneMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(1.2f, 1.2f, 1.2f);
    mBackWall->LightProjector = mLightProjector;
    mSceneObjects->AddRenderObject(mBackWall);

    material = new Material(mtVPL);
	mLeftWall = new SceneMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.2f, 0.2f);
    mLeftWall->LightProjector = mLightProjector;
    mSceneObjects->AddRenderObject(mLeftWall);

    material = new Material(mtVPL);
	mRightWall = new SceneMesh(material, mCamera);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource();
	rotM = RotateZ(90.0f);
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.2f, 1.0f, 0.2f);
    mRightWall->LightProjector = mLightProjector;
    mSceneObjects->AddRenderObject(mRightWall);

    // Create screen quads.
    material = new Material(mtScreenQuad);
    mTempScreenQuad = new TempScreenQuad(material);
    mTempScreenQuad->LoadFromFile("screenquad.ply");
    mTempScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
    mTempScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
    mTempScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
    mTempScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
    mTempScreenQuad->CreateDeviceResource();
    mTempScreenQuad->ShowMode = 2;
    mTempScreenQuad->TempTexture = mIndirectLightingTexture;
    mTempScreenQuad->TempTexture2 = mDirectLightingTexture;
    mTempScreenQuad->TempTextureArray = mRSMFluxTextureArray;

    // Create labels.
    InformationPanel::GetInstance()->AddTimingLabel("Scene Shadow Pass", 16, 20);
    InformationPanel::GetInstance()->AddTimingLabel("Scene G-buffer Pass", 16, 40);
    InformationPanel::GetInstance()->AddTimingLabel("RSM Pass", 16, 60);
    InformationPanel::GetInstance()->AddTimingLabel("VPL Creation Pass", 16, 80);
    InformationPanel::GetInstance()->AddTimingLabel("Direct Lighting Pass", 16, 100);
    InformationPanel::GetInstance()->AddTimingLabel("Indirect Lighting Pass", 16, 120);
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::Run()
{
    static float angle = 0.0f;
    if( mIsRotatingModel )
    {
        angle += 1.0f;
        mat4 rot;
        rot = RotateY(angle);
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

    // Scene shadow pass.
    mShadowMapRenderer->Render(0, 0, mLightProjector);
    workLoad = mShadowMapRenderer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Scene Shadow Pass", workLoad);

    // Scene G-buffer pass.
    mGBufferRenderer->Render(0, 1, mCamera);
    workLoad = mGBufferRenderer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Scene G-buffer Pass", workLoad);

    // Scene light RSM pass.
    mRSMRenderer->Render(0, 2, 0);
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
    
	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::Terminate()
{
	// Release all resources.

	delete mCamera;
    delete mLightProjector;

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
void BidirectionalVoxelGIApp::OnKeyboard(unsigned char key, int x, int y)
{
    switch( key )
    {
    case '1':
        mShowMode = SM_Shadow;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mShadowMapTexture;
        break;

    case '2':
        mShowMode = SM_GBufferPosition;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mGBufferPositionTexture;
        break;

    case '3':
        mShowMode = SM_GBufferNormal;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mGBufferNormalTexture;
        break;

    case '4':
        mShowMode = SM_GBufferAlbedo;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mGBufferAlbedoTexture;
        break;

    case '5':
        mShowMode = SM_RSMPosition;
        mTempScreenQuad->ShowMode = 1;
        mTempScreenQuad->TempTextureArray = mRSMPositionTextureArray;
        break;

    case '6':
        mShowMode = SM_RSMNormal;
        mTempScreenQuad->ShowMode = 1;
        mTempScreenQuad->TempTextureArray = mRSMNormalTextureArray;
        break;

    case '7':
        mShowMode = SM_RSMFlux;
        mTempScreenQuad->ShowMode = 1;
        mTempScreenQuad->TempTextureArray = mRSMFluxTextureArray;
        break;

    case '8':
        mShowMode = SM_DirectLighting;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mDirectLightingTexture;
        break;

    case '9':
        mShowMode = SM_IndirectLighting;
        mTempScreenQuad->ShowMode = 0;
        mTempScreenQuad->TempTexture = mIndirectLightingTexture;
        break;

    case '0':
        mShowMode = SM_Final;
        mTempScreenQuad->ShowMode = 2;
        mTempScreenQuad->TempTexture = mIndirectLightingTexture;
        mTempScreenQuad->TempTexture2 = mDirectLightingTexture;
        break;

    case 'X':
        mTempScreenQuad->TextureArrayIndex = 0;
        break;

    case 'x':
        mTempScreenQuad->TextureArrayIndex = 1;
        break;

    case 'Y':
        mTempScreenQuad->TextureArrayIndex = 2;
        break;

    case 'y':
        mTempScreenQuad->TextureArrayIndex = 3;
        break;

    case 'Z':
        break;

    case 'z':
        mTempScreenQuad->TextureArrayIndex = 4;
        break;

    case 'w':
        mIsWireframe = !mIsWireframe;
        break;

    case 'r':
        mIsRotatingModel = !mIsRotatingModel;
        break;

    case '=':
        if( mShowMode == SM_VPLShadow )
        {
            mTempScreenQuad->TextureArrayIndex++;
            mTempScreenQuad->TextureArrayIndex = mTempScreenQuad->TextureArrayIndex % VPL_SAMPLE_COUNT;
        }
        break;

    case '-':
        if( mShowMode == SM_VPLShadow )
        {
            mTempScreenQuad->TextureArrayIndex--;
            mTempScreenQuad->TextureArrayIndex = mTempScreenQuad->TextureArrayIndex % VPL_SAMPLE_COUNT;
        }
        break;

    default:
        break;
    }
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void BidirectionalVoxelGIApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------