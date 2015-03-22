#include "VPLviaSVOGIApp.h"
#include "InformationPanel.h"
#include <glfw3.h>

using namespace RTGI;
using namespace RTGI::GUIFramework;

//#define SHOW_TIMING

//----------------------------------------------------------------------------
VPLviaSVOGI::VPLviaSVOGI(int width, int height)
{
	Width = width;
	Height = height;
	Title = "VPL via SVO GI";
    mIsRotatingModel = false;
    mIsWireframe = false;
    mUseTC = false;
    mVoxelizerType = Voxelizer::VT_SVO;
}
//----------------------------------------------------------------------------
VPLviaSVOGI::~VPLviaSVOGI()
{
}
//----------------------------------------------------------------------------
void VPLviaSVOGI::Initialize(GPUDevice* device)
{
	float color = 0.0f;
    glClearColor(color, color, color, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create scene camera.
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width/(float)Height, 0.01f, 150.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    // Create light.
    ShaderProgramInfo lightMeshProgramInfo;
    lightMeshProgramInfo.VShaderFileName = "VPLviaSVOGI/vLightMesh.glsl";
    lightMeshProgramInfo.FShaderFileName = "VPLviaSVOGI/fLightMesh.glsl";
    lightMeshProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                           ShaderType::ST_Fragment;
    Pass* passLightMesh = new Pass(lightMeshProgramInfo);
    Technique* techLightMesh = new Technique();
    techLightMesh->AddPass(passLightMesh);
    MaterialTemplate* mtLightMesh = new MaterialTemplate();
    mtLightMesh->AddTechnique(techLightMesh);

    Camera* lightProjector = new Camera();
    lightProjector->SetPerspectiveFrustum(85.0f, 1.0f, 0.01f, 50.0f);
    lightProjector->SetLookAt(vec3(0.0f, 10.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f));
    mLight = new Light();
    mLight->SetProjector(lightProjector);

	// Create material templates.
	Material* material = 0;

    ShaderProgramInfo voxelizationProgramInfo;
    voxelizationProgramInfo.VShaderFileName = "VPLviaSVOGI/vVoxelization.glsl";
    voxelizationProgramInfo.GShaderFileName = "VPLviaSVOGI/gVoxelization.glsl";
    if( mVoxelizerType == Voxelizer::VT_Grid )
    {
        voxelizationProgramInfo.FShaderFileName = "VPLviaSVOGI/fVoxelization.glsl";
    }
    else if( mVoxelizerType == Voxelizer::VT_SVO )
    {
        voxelizationProgramInfo.FShaderFileName = "VPLviaSVOGI/fSVOVoxelization.glsl";
    }
    else
    {
        assert(false);
    }
    voxelizationProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                              ShaderType::ST_Geometry |
                                              ShaderType::ST_Fragment;
    Pass* passVoxelization = new Pass(voxelizationProgramInfo);

    ShaderProgramInfo shadowProgramInfo;
    shadowProgramInfo.VShaderFileName = "VPLviaSVOGI/vShadow.glsl";
    shadowProgramInfo.FShaderFileName = "VPLviaSVOGI/fShadow.glsl";
    shadowProgramInfo.TCShaderFileName = "VPLviaSVOGI/tcShadow.glsl";
    shadowProgramInfo.TEShaderFileName = "VPLviaSVOGI/teShadow.glsl";
    shadowProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                        ShaderType::ST_Fragment | 
                                        ShaderType::ST_TessellationControl |
                                        ShaderType::ST_TessellationEvaluation;
    Pass* passShadow = new Pass(shadowProgramInfo);

    ShaderProgramInfo gbufferProgramInfo;
    if( mUseTC )
    {
        gbufferProgramInfo.VShaderFileName = "VPLviaSVOGI/vGBufferRPC.glsl";
        gbufferProgramInfo.FShaderFileName = "VPLviaSVOGI/fGBufferRPC.glsl";
    }
    else
    {
        gbufferProgramInfo.VShaderFileName = "VPLviaSVOGI/vGBuffer.glsl";
        gbufferProgramInfo.FShaderFileName = "VPLviaSVOGI/fGBuffer.glsl";
    }
    gbufferProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                         ShaderType::ST_Fragment;
    Pass* passGBuffer = new Pass(gbufferProgramInfo);

    ShaderProgramInfo rsmProgramInfo;
    rsmProgramInfo.VShaderFileName = "VPLviaSVOGI/vRSM.glsl";
    rsmProgramInfo.GShaderFileName = "VPLviaSVOGI/gRSM.glsl";
    rsmProgramInfo.FShaderFileName = "VPLviaSVOGI/fRSM.glsl";
    rsmProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                     ShaderType::ST_Geometry |
                                     ShaderType::ST_Fragment;
    Pass* passRSM = new Pass(rsmProgramInfo);

	Technique* techSceneModel = new Technique();
    techSceneModel->AddPass(passVoxelization);
    techSceneModel->AddPass(passShadow);
    techSceneModel->AddPass(passGBuffer);
    techSceneModel->AddPass(passRSM);
	MaterialTemplate* mtSceneModel = new MaterialTemplate();
    mtSceneModel->AddTechnique(techSceneModel);

    // Create scene.
    mat4 rotM;
    material = new Material(mtSceneModel);
    mModel = new SceneMesh(material, mMainCamera);
    mModel->LoadFromFile("happy_s.ply");
    mat4 scale = glm::scale(mat4(), vec3(45.0f));
    mModel->UpdateModelSpaceVertices(scale);
    mModel->GenerateNormals();
    mModel->CreateDeviceResource(mDevice);
    mModel->SetWorldTranslation(vec3(-4.0f, 4.2f, 1.0f));
    mModel->MaterialColor = vec3(1.8f, 1.8f, 1.8f);
    mModel->LightProjector = mLight->GetProjector();
    mModel->SceneBB = &mSceneBB;
    mSceneBB.Merge(mModel->GetWorldSpaceBB());

    material = new Material(mtSceneModel);
    mModel2 = new SceneMesh(material, mMainCamera);
    mModel2->LoadFromFile("dragon_s.ply");
    scale = glm::scale(mat4(), vec3(45.0f));
    mModel2->UpdateModelSpaceVertices(scale);
    mModel2->GenerateNormals();
    mModel2->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(-60.0f), vec3(0, 1, 0));
    mModel2->SetWorldTransform(rotM);
    mModel2->SetWorldTranslation(vec3(3.2f, 3.2f, 2.4f));
    mModel2->MaterialColor = vec3(0.8f, 1.0f, 2.0f);
    mModel2->LightProjector = mLight->GetProjector();
    mModel2->SceneBB = &mSceneBB;
    mSceneBB.Merge(mModel2->GetWorldSpaceBB());

    material = new Material(mtSceneModel);
    mGround = new SceneMesh(material, mMainCamera);
    mGround->LoadFromFile("square.ply");
    mGround->GenerateNormals();
    mGround->CreateDeviceResource(mDevice);
    mGround->MaterialColor = vec3(1.5f, 1.5f, 1.5f);
    mGround->LightProjector = mLight->GetProjector();
    mGround->SceneBB = &mSceneBB;
    mSceneBB.Merge(mGround->GetWorldSpaceBB());

    material = new Material(mtSceneModel);
    mCeiling = new SceneMesh(material, mMainCamera);
    mCeiling->LoadFromFile("square.ply");
    mCeiling->GenerateNormals();
    mCeiling->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(180.0f), vec3(1, 0, 0));
    mCeiling->SetWorldTransform(rotM);
    mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(1.5f, 1.5f, 1.5f);
    mCeiling->LightProjector = mLight->GetProjector();
    mCeiling->SceneBB = &mSceneBB;
    mSceneBB.Merge(mCeiling->GetWorldSpaceBB());

    material = new Material(mtSceneModel);
    mBackWall = new SceneMesh(material, mMainCamera);
    mBackWall->LoadFromFile("square.ply");
    mBackWall->GenerateNormals();
    mBackWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
    mBackWall->SetWorldTransform(rotM);
    mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(1.5f, 1.5f, 1.5f);
    mBackWall->LightProjector = mLight->GetProjector();
    mBackWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mBackWall->GetWorldSpaceBB());

    material = new Material(mtSceneModel);
    mLeftWall = new SceneMesh(material, mMainCamera);
    mLeftWall->LoadFromFile("square.ply");
    mLeftWall->GenerateNormals();
    mLeftWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(-90.0f), vec3(0, 0, 1));
    mLeftWall->SetWorldTransform(rotM);
    mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.2f, 0.2f);
    mLeftWall->LightProjector = mLight->GetProjector();
    mLeftWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mLeftWall->GetWorldSpaceBB());

    material = new Material(mtSceneModel);
    mRightWall = new SceneMesh(material, mMainCamera);
    mRightWall->LoadFromFile("square.ply");
    mRightWall->GenerateNormals();
    mRightWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(0, 0, 1));
    mRightWall->SetWorldTransform(rotM);
    mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.2f, 1.0f, 0.2f);
    mRightWall->LightProjector = mLight->GetProjector();
    mRightWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mRightWall->GetWorldSpaceBB());

    // Create scene renderset.
    mSceneObjects = new RenderSet();
    mSceneObjects->AddRenderObject(mModel);
    mSceneObjects->AddRenderObject(mModel2);
    mSceneObjects->AddRenderObject(mGround);
    mSceneObjects->AddRenderObject(mCeiling);
    mSceneObjects->AddRenderObject(mBackWall);
    mSceneObjects->AddRenderObject(mLeftWall);
    mSceneObjects->AddRenderObject(mRightWall);
    mSceneObjects->UpdateRenderCache();

    // Create voxelizer renderset.
    mVoxelizedObjects = new RenderSet();
    if( mVoxelizerType == Voxelizer::VT_Grid )
    {
        mVoxelizedObjects->AddRenderObject(mModel);
        mVoxelizedObjects->AddRenderObject(mModel2);
        mVoxelizedObjects->AddRenderObject(mGround);
        mVoxelizedObjects->AddRenderObject(mCeiling);
        mVoxelizedObjects->AddRenderObject(mBackWall);
        mVoxelizedObjects->AddRenderObject(mLeftWall);
        mVoxelizedObjects->AddRenderObject(mRightWall);
    }
    else if( mVoxelizerType == Voxelizer::VT_SVO )
    {
        mVoxelizedObjects->AddRenderObject(mModel);
        mVoxelizedObjects->AddRenderObject(mModel2);
    }
    else
    {
        assert(false);
    }

    // Create scene voxelizer.
    if( mVoxelizerType == Voxelizer::VT_Grid )
    {
        mVoxelizer = new GridVoxelizer(mDevice);
        ((GridVoxelizer*)(Voxelizer*)mVoxelizer)->Initialize(mDevice, 
            VOXEL_DIMENSION, VOXEL_LOCAL_GROUP_DIM, &mSceneBB);
    }
    else if( mVoxelizerType == Voxelizer::VT_SVO )
    {
        mVoxelizer = new SVOVoxelizer(mDevice);
        ((SVOVoxelizer*)(Voxelizer*)mVoxelizer)->Initialize(mDevice, 
            VOXEL_DIMENSION, &mSceneBB);
    }
    else
    {
        assert(false);
    }
    mVoxelizer->RasterizerDimBias = -1;
    mVoxelizer->SetRenderSet(mVoxelizedObjects);

    // Create G-buffer renderer.
    GBufferDesc gbufferDesc;
    gbufferDesc.Width = Width;
    gbufferDesc.Height = Height;
    gbufferDesc.PositionFormat = BF_RGBAF;
    gbufferDesc.NormalFormat = BF_RGBAF;
    gbufferDesc.AlbedoFormat = BF_RGBAF;
    gbufferDesc.RPCFormat = BF_RGBAF;
    mGBufferRenderer = new GBufferRenderer(mDevice);
    mGBufferRenderer->CreateGBuffer(&gbufferDesc);
    mGBufferRenderer->SetRenderSet(mSceneObjects);

    // Create shadow map renderer.
    mShadowMapRenderer = new ShadowMapRenderer(mDevice);
    mShadowMapRenderer->CreateShadowMap(1024, 1024, BF_RGBAF);
    mShadowMapRenderer->SetRenderSet(mSceneObjects);

    // Create RSM renderer.
    mRSMRenderer = new RSMRenderer(mDevice);
    mRSMRenderer->CreateRSM(256, 256, RSM_FACE_COUNT, BF_RGBAF);
    mRSMRenderer->SetRenderSet(mSceneObjects);

    // Create VPL generator.
    mVPLGenerator = new VPLGenerator(mDevice);
    mVPLGenerator->SetRSMRenderer(mRSMRenderer);
    mVPLGenerator->Initialize(mDevice, VPL_SAMPLE_COUNT);

    // Create direct lighting renderer.
    mDirectLightingRenderer = new DirectLightingRenderer(mDevice);
    mDirectLightingRenderer->Initialize(mDevice, Width, Height, BF_RGBAF, 
        mLight->GetProjector(), mGBufferRenderer, mShadowMapRenderer);

    // Create indirect lighting renderer.
    mIndirectLightingRenderer = new IndirectLightingRenderer(mDevice);
    mIndirectLightingRenderer->Initialize(mDevice, Width, Height, BF_RGBAF, 
        VPL_SAMPLE_COUNT, INTERLEAVED_PATTERN_SIZE, &mSceneBB, VOXEL_DIMENSION, 
        mGBufferRenderer, mVPLGenerator, mVoxelizer, mUseTC);

    // Create visualizer.
    mVisualizer = new Visualizer(mDevice);
    mVisualizer->Initialize(mDevice, mVoxelizer, mVPLGenerator, 
        mShadowMapRenderer, mGBufferRenderer, mRSMRenderer, 
        mDirectLightingRenderer, mIndirectLightingRenderer, &mSceneBB, 
        VOXEL_DIMENSION, VOXEL_LOCAL_GROUP_DIM, mMainCamera, 
        INTERLEAVED_PATTERN_SIZE, VPL_SAMPLE_COUNT);

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
    mVisualizer->SetTimer(mTimer);

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
#ifdef SHOW_TIMING
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
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddTimingLabel("Visualizer Pass", 16, infoStartY);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddTimingLabel("Total", 16, infoStartY);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddTimingLabel("Frame Counter", 16, infoStartY);
#endif

    infoStartY = 20;
    InformationPanel::GetInstance()->AddRadioButton("Voxel Buffer", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    if( mVoxelizerType == Voxelizer::VT_Grid )
    {
        InformationPanel::GetInstance()->AddRadioButton("Voxel Grid", 16, infoStartY, 60, 20, false);
    }
    else if( mVoxelizerType == Voxelizer::VT_SVO )
    {
        InformationPanel::GetInstance()->AddRadioButton("SVO Grid", 16, infoStartY, 60, 20, false);
    }
    else
    {
        assert(false);
    }
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("Scene Shadow Map", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("G-Buffer Position", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("G-Buffer Normal", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("G-Buffer Albedo", 16, infoStartY, 60, 20, false);
    if( mUseTC )
    {
        infoStartY += infoIncY;
        InformationPanel::GetInstance()->AddRadioButton("G-Buffer RPC", 16, infoStartY, 60, 20, false);
    }
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
    InformationPanel::GetInstance()->AddRadioButton("Filtered Indirect Lighting", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddRadioButton("Final Result", 16, infoStartY, 60, 20, true);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddCheckBox("Show Direct Shadow", 16, infoStartY, 60, 20, true);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddCheckBox("VPL Visibility Test", 16, infoStartY, 60, 20, true);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddCheckBox("Show VPL", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddCheckBox("Show VPL Interleaved Sampling Subset", 16, infoStartY, 60, 20, false);
    infoStartY += infoIncY;
    InformationPanel::GetInstance()->AddCheckBox("Show VPL Flux Contrast", 16, infoStartY, 60, 20, false);
    infoStartY += 24;
    InformationPanel::GetInstance()->AddButton("Prev VPL Subset", 16, infoStartY, 100, 24);
    InformationPanel::GetInstance()->AddButton("Next VPL Subset", 120, infoStartY, 100, 24);
}
//----------------------------------------------------------------------------
void VPLviaSVOGI::FrameFunc()
{
    mModel->UpdateRenderCache();

    static float angle = 0.0f;
    static float angle2 = -60.0f;
    if( mIsRotatingModel )
    {
        angle += 1.0f;
        mat4 rot;
        rot = rotate(mat4(), radians(angle), vec3(0, 1, 0));
        vec3 trans = mModel->GetWorldTranslation();
        mModel->SetWorldTransform(rot);
        mModel->SetWorldTranslation(trans);

        angle2 -= 4.0f;
        rot = rotate(mat4(), radians(angle2), vec3(0, 1, 0));
        trans = mModel2->GetWorldTranslation();
        mModel2->SetWorldTransform(rot);
        mModel2->SetWorldTranslation(trans);
    }

    if( mIsWireframe )
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

#ifdef SHOW_TIMING
    InformationPanel^ infoPanel = InformationPanel::GetInstance();
    static double workLoad;
    static double totalWorkLoad;
    totalWorkLoad = 0.0;
#endif

    // Scene voxelization pass.
    mVoxelizer->Render(0, SMP_Voxelization);
#ifdef SHOW_TIMING
    workLoad = mVoxelizer->GetTimeElapsed();
    totalWorkLoad += workLoad;
    infoPanel->SetTimingLabelValue("Scene Voxelization Pass", workLoad);
#endif

    // Scene shadow pass.
    mShadowMapRenderer->Render(0, SMP_ShadowMap, mLight->GetProjector());
#ifdef SHOW_TIMING
    workLoad = mShadowMapRenderer->GetTimeElapsed();
    totalWorkLoad += workLoad;
    infoPanel->SetTimingLabelValue("Scene Shadow Pass", workLoad);
#endif

    // Scene G-buffer pass.
    mGBufferRenderer->Render(0, SMP_GBuffer, mMainCamera);
#ifdef SHOW_TIMING
    workLoad = mGBufferRenderer->GetTimeElapsed();
    totalWorkLoad += workLoad;
    infoPanel->SetTimingLabelValue("Scene G-buffer Pass", workLoad);
#endif

    // Scene light RSM pass.
    mRSMRenderer->Render(0, SMP_RSM, 0);
#ifdef SHOW_TIMING
    workLoad = mRSMRenderer->GetTimeElapsed();
    totalWorkLoad += workLoad;
    infoPanel->SetTimingLabelValue("RSM Pass", workLoad);
#endif

    // Sample RSM pass (VPL generation).
    mVPLGenerator->Run();
#ifdef SHOW_TIMING
    workLoad = mVPLGenerator->GetTimeElapsed();
    totalWorkLoad += workLoad;
    infoPanel->SetTimingLabelValue("VPL Creation Pass", workLoad);
#endif

    // Deferred direct illumination pass.
    mDirectLightingRenderer->Render();
#ifdef SHOW_TIMING
    workLoad = mDirectLightingRenderer->GetTimeElapsed();
    totalWorkLoad += workLoad;
    infoPanel->SetTimingLabelValue("Direct Lighting Pass", workLoad);
#endif

    // Deferred indirect illumination pass.
    mIndirectLightingRenderer->Render();
#ifdef SHOW_TIMING
    workLoad = mIndirectLightingRenderer->GetTimeElapsed();
    totalWorkLoad += workLoad;
    infoPanel->SetTimingLabelValue("Indirect Lighting Pass", workLoad);
#endif

    // Show rendering result.
    mVisualizer->Render(0, 0);
#ifdef SHOW_TIMING
    workLoad = mVisualizer->GetTimeElapsed();
    totalWorkLoad += workLoad;
    infoPanel->SetTimingLabelValue("Visualizer Pass", workLoad);
    infoPanel->SetTimingLabelValue("Total", totalWorkLoad);
    infoPanel->SetTimingLabelValue("Frame Counter", FrameCounter);
#endif
}
//----------------------------------------------------------------------------
void VPLviaSVOGI::Terminate()
{
	// Release all resources.
    mLight = 0;

    mVoxelizer = 0;
    mGBufferRenderer = 0;
    mShadowMapRenderer = 0;
    mDirectLightingRenderer = 0;
    mIndirectLightingRenderer = 0;
    mRSMRenderer = 0;
    mVPLGenerator = 0;
    mVisualizer = 0;

	mGround = 0;
	mCeiling = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;
    mModel2 = 0;
    mSceneObjects = 0;
    mVoxelizedObjects = 0;

    mTimer = 0;
}
//----------------------------------------------------------------------------
void VPLviaSVOGI::ProcessInput()
{
	if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS)
	{
		mIsRotatingModel = !mIsRotatingModel;
	}
	if (glfwGetKey(Window, GLFW_KEY_T) == GLFW_PRESS)
	{
		mIsWireframe = !mIsWireframe;
	}
}
//----------------------------------------------------------------------------
void VPLviaSVOGI::OnRadioButtonClick(System::Object^ sender, 
    System::EventArgs^ e)
{
    RadioButton^ radioButton = (RadioButton^)sender;
    if( !mVisualizer )
    {
        return;
    }

    if( radioButton->Name == "Voxel Buffer" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_VoxelBuffer);
    }

    if( radioButton->Name == "Voxel Grid" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_VoxelGrid);
    }

    if( radioButton->Name == "SVO Grid" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_SVOGrid);
    }

    if( radioButton->Name == "Scene Shadow Map" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_Shadow);
    }

    if( radioButton->Name == "G-Buffer Position" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_GBufferPosition);
    }

    if( radioButton->Name == "G-Buffer Normal" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_GBufferNormal);
    }

    if( radioButton->Name == "G-Buffer Albedo" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_GBufferAlbedo);
    }

    if( radioButton->Name == "G-Buffer RPC" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_GBufferRPC);
    }

    if( radioButton->Name == "RSM Position" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_RSMPosition);
    }

    if( radioButton->Name == "RSM Normal" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_RSMNormal);
    }

    if( radioButton->Name == "RSM Flux" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_RSMFlux);
    }

    if( radioButton->Name == "Direct Lighting" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_DirectLighting);
    }

    if( radioButton->Name == "Indirect Lighting" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_IndirectLighting);
    }

    if( radioButton->Name == "Filtered Indirect Lighting" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_FilteredIndirectLighting);
    }

    if( radioButton->Name == "Final Result" )
    {
        mVisualizer->SetShowMode(Visualizer::SM_Final);
    }
}
//----------------------------------------------------------------------------
void VPLviaSVOGI::OnCheckBoxClick(System::Object^ sender, 
    System::EventArgs^ e)
{
    CheckBox^ checkBox = (CheckBox^)sender;

    if( checkBox->Name == "Show Direct Shadow" )
    {
        if( !mDirectLightingRenderer )
        {
            return;
        }

        mDirectLightingRenderer->ShowShadow(checkBox->Checked);
    }

    if( checkBox->Name == "VPL Visibility Test" )
    {
        if( !mIndirectLightingRenderer )
        {
            return;
        }

        mIndirectLightingRenderer->VPLVisibilityTest(checkBox->Checked);
    }

    if( checkBox->Name == "Show VPL" )
    {
        if( !mVisualizer )
        {
            return;
        }

        mVisualizer->SetShowVPL(checkBox->Checked);
    }

    if( checkBox->Name == "Show VPL Interleaved Sampling Subset" )
    {
        if( !mVisualizer )
        {
            return;
        }

        mVisualizer->SetShowVPLSubset(checkBox->Checked);
    }

    if( checkBox->Name == "Show VPL Flux Contrast" )
    {
        if( !mVisualizer )
        {
            return;
        }

        mVisualizer->SetShowVPLFluxContrast(checkBox->Checked);
    }
}
//----------------------------------------------------------------------------
void VPLviaSVOGI::OnButtonClick(System::Object^  sender,
    System::EventArgs^  e)
{
    Button^ button = (Button^)sender;

    if( button->Name == "Prev VPL Subset" )
    {
        int curVPLSubsetIndex = mVisualizer->GetCurVPLSubsetIndex();
        --curVPLSubsetIndex;
        mVisualizer->SetCurVPLSubsetIndex(curVPLSubsetIndex);
    }

    if( button->Name == "Next VPL Subset" )
    {
        int curVPLSubsetIndex = mVisualizer->GetCurVPLSubsetIndex();
        ++curVPLSubsetIndex;
        mVisualizer->SetCurVPLSubsetIndex(curVPLSubsetIndex);
    }
}
//----------------------------------------------------------------------------