#include "SimpleVoxelizationApp.h"
#include <glfw3.h>
using namespace RTGI;
using namespace RTGI::GUIFramework;

float SimpleVoxelizationApp::RaySegment[6] = { 0.0f, 0.0f, 0.0f, 
                                               0.0f, 0.0f, 0.0f };

//#define DEBUG_VOXEL_RAY_INTERSECTION

//----------------------------------------------------------------------------
SimpleVoxelizationApp::SimpleVoxelizationApp(int width, int height)
{
	Width = width;
	Height = height;
	Title = "Simple voxelization demo";
    mIsRotatingModel = false;
    mShowMode = SM_VoxelGrid;
}
//----------------------------------------------------------------------------
SimpleVoxelizationApp::~SimpleVoxelizationApp()
{
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::Initialize(GPUDevice* device)
{
    GLint globalX, globalY, globalZ;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &globalX);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &globalY);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &globalZ);
    assert(globalX >= VOXEL_DIMENSION / LOCAL_GROUP_DIM &&
           globalY >= VOXEL_DIMENSION / LOCAL_GROUP_DIM &&
           globalZ >= VOXEL_DIMENSION / LOCAL_GROUP_DIM);

	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   
    // Create scene camera.
    mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width / (float)Height, 1.0f, 50.0f);
    mMainCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    // Create voxelization projector.
	mVoxelizationProjector = new Camera(false);
    mVoxelizationProjector->SetOrthogonalFrustum(10.5f, 1.0f, 0.01f, 20.5f);

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo voxelizationProgramInfo;
    voxelizationProgramInfo.VShaderFileName = "SimpleVoxelization/vVoxelization.glsl";
    voxelizationProgramInfo.GShaderFileName = "SimpleVoxelization/gVoxelization.glsl";
    voxelizationProgramInfo.FShaderFileName = "SimpleVoxelization/fVoxelization.glsl";
    voxelizationProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                              ShaderType::ST_Geometry | 
                                              ShaderType::ST_Fragment;
    Pass* passVoxelization = new Pass(voxelizationProgramInfo);

    ShaderProgramInfo showVoxelizationProgramInfo;
    showVoxelizationProgramInfo.VShaderFileName = "SimpleVoxelization/vShowVoxelization.glsl";
    showVoxelizationProgramInfo.FShaderFileName = "SimpleVoxelization/fShowVoxelization.glsl";
    showVoxelizationProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                                  ShaderType::ST_Fragment;
    Pass* passShowVoxelization = new Pass(showVoxelizationProgramInfo);

    Technique* techVoxelization = new Technique();
    techVoxelization->AddPass(passVoxelization);
    techVoxelization->AddPass(passShowVoxelization);
    MaterialTemplate* mtVoxelization = new MaterialTemplate();
    mtVoxelization->AddTechnique(techVoxelization);

    ShaderProgramInfo showVoxelGridProgramInfo;
    showVoxelGridProgramInfo.VShaderFileName = "SimpleVoxelization/vShowVoxelGrid.glsl";
    showVoxelGridProgramInfo.FShaderFileName = "SimpleVoxelization/fShowVoxelGrid.glsl";
    showVoxelGridProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                               ShaderType::ST_Fragment;
    Pass* passShowVoxelGrid = new Pass(showVoxelGridProgramInfo);

    Technique* techShowVoxelGrid = new Technique();
    techShowVoxelGrid->AddPass(passShowVoxelGrid);
    MaterialTemplate* mtShowVoxelGrid = new MaterialTemplate();
    mtShowVoxelGrid->AddTechnique(techShowVoxelGrid);

    // Create reset voxel buffer task.
    ShaderProgramInfo resetVoxelBufferProgramInfo;
    resetVoxelBufferProgramInfo.CShaderFileName = "SimpleVoxelization/cResetVoxelBuffer.glsl";
    resetVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passResetVoxelBuffer = new ComputePass(resetVoxelBufferProgramInfo);
    mResetVoxelBufferTask = new ResetVoxelBuffer();
    mResetVoxelBufferTask->AddPass(passResetVoxelBuffer);
    mResetVoxelBufferTask->CreateDeviceResource(mDevice);

    // Create gather voxel buffer task.
    ShaderProgramInfo gatherVoxelBufferProgramInfo;
    gatherVoxelBufferProgramInfo.CShaderFileName = "SimpleVoxelization/cGatherVoxelBuffer.glsl";
    gatherVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passGatherVoxelBuffer = new ComputePass(gatherVoxelBufferProgramInfo);
    mGatherVoxelBufferTask = new GatherVoxelBuffer();
    mGatherVoxelBufferTask->AddPass(passGatherVoxelBuffer);
    mGatherVoxelBufferTask->CreateDeviceResource(mDevice);
    mGatherVoxelBufferTask->SceneBB = &mSceneBB;

    // Create voxel grid intersection task.
    ShaderProgramInfo voxelGridIntersectionProgramInfo;
    voxelGridIntersectionProgramInfo.CShaderFileName = "SimpleVoxelization/cVoxelGridIntersection.glsl";
    voxelGridIntersectionProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passVoxelGridIntersection = new ComputePass(voxelGridIntersectionProgramInfo);
    mVoxelGridIntersectionTask = new VoxelGridIntersection();
    mVoxelGridIntersectionTask->AddPass(passVoxelGridIntersection);
    mVoxelGridIntersectionTask->CreateDeviceResource(mDevice);
    mVoxelGridIntersectionTask->SceneBB = &mSceneBB;

    // Create scene voxel buffer.
    mVoxelBuffer = new StructuredBuffer();
    GLuint voxelCount = VOXEL_DIMENSION * VOXEL_DIMENSION * VOXEL_DIMENSION;
    GLuint bufferSize = voxelCount * sizeof(GLuint) * 4;
    mVoxelBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Copy);
    memset(mZeroBuffer, 0x00, bufferSize);

    // Create indirect command buffer.
    mIndirectCommandBuffer = new StructuredBuffer();
    bufferSize = sizeof(GLuint)*5 + sizeof(GLfloat)*35 + voxelCount*sizeof(GLfloat)*4;
    mIndirectCommandBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Copy);

    // Create gathered voxel GPU memory allocator counter.
    mGatheredVoxelAllocCounter = new AtomicCounterBuffer();
    mGatheredVoxelAllocCounter->ReserveMutableDeviceResource(mDevice, sizeof(GLuint), BU_Dynamic_Copy);

	// Create scene.
	mat4 rotM;
    material = new Material(mtVoxelization);
	mModel = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mModel->LoadFromPLYFile("dragon_s.ply");
    mat4 scale = glm::scale(mat4(), vec3(60.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource(mDevice);
    mModel->SetWorldTranslation(vec3(0.0f, 4.4f, 3.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);
    mModel->SceneBB = &mSceneBB;
    mSceneBB.Merge(mModel->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
	mGround = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mGround->LoadFromPLYFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource(mDevice);
    mGround->MaterialColor = vec3(0.8f, 0.8f, 0.0f);
    mGround->SceneBB = &mSceneBB;
    mSceneBB.Merge(mGround->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
	mCeiling = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mCeiling->LoadFromPLYFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource(mDevice);
	rotM = rotate(mat4(), radians(180.0f), vec3(1, 0, 0));
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);
    mCeiling->SceneBB = &mSceneBB;
    mSceneBB.Merge(mCeiling->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
	mBackWall = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mBackWall->LoadFromPLYFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    mBackWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mBackWall->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
	mLeftWall = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mLeftWall->LoadFromPLYFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(-90.0f), vec3(0, 0, 1));
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
    mLeftWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mLeftWall->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
	mRightWall = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mRightWall->LoadFromPLYFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(0, 0, 1));
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);
    mRightWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mRightWall->GetWorldSpaceBB());

    // Create voxel cube model.
    material = new Material(mtShowVoxelGrid);
    mVoxelCubeModel = new VoxelCubeTriMesh(material, mMainCamera);
    mVoxelCubeModel->LoadFromPLYFile("box.ply");
    mVoxelCubeModel->GenerateNormals();
    mVoxelCubeModel->SetIndirectCommandBuffer(mIndirectCommandBuffer, 0);
    mVoxelCubeModel->CreateDeviceResource(mDevice);

	// Create information panel.
	int screenX, screenY;
	glfwGetWindowPos(Window, &screenX, &screenY);
	InformationPanel^ infoPanel = gcnew InformationPanel();
	infoPanel->Show();
	infoPanel->SetDesktopLocation(screenX + Width + 12, screenY - 30);

    // Create GUI elements.
    InformationPanel::GetInstance()->AddListener(this);
    InformationPanel::GetInstance()->AddTimingLabel("Reset Voxel Buffer Pass", 16, 20);
    InformationPanel::GetInstance()->AddTimingLabel("Voxelization Pass", 16, 40);
    InformationPanel::GetInstance()->AddTimingLabel("Voxel Count", 16, 60);
    InformationPanel::GetInstance()->AddTimingLabel("Reset Counter Time", 16, 80);
    InformationPanel::GetInstance()->AddTimingLabel("Counter", 16, 100);
    InformationPanel::GetInstance()->AddTimingLabel("Visualization Pass", 16, 120);
    InformationPanel::GetInstance()->AddTimingLabel("Intersection Pass", 16, 140);
    InformationPanel::GetInstance()->AddTextBox("P1:", 16, 20, 120, 16);
    InformationPanel::GetInstance()->AddTextBox("P2:", 16, 44, 120, 16);
    InformationPanel::GetInstance()->AddButton("Create Ray", 60, 80, 80, 24);

#ifdef DEBUG_VOXEL_RAY_INTERSECTION
    InformationPanel::GetInstance()->AddDebugLabel("Ray Direction X", 16, 20);
    InformationPanel::GetInstance()->AddDebugLabel("Ray Direction Y", 16, 40);
    InformationPanel::GetInstance()->AddDebugLabel("Ray Direction Z", 16, 60);
    InformationPanel::GetInstance()->AddDebugLabel("Voxel Extension X", 16, 80);
    InformationPanel::GetInstance()->AddDebugLabel("Voxel Extension Y", 16, 100);
    InformationPanel::GetInstance()->AddDebugLabel("Voxel Extension Z", 16, 120);
    InformationPanel::GetInstance()->AddDebugLabel("Ray maxT", 16, 140);
    InformationPanel::GetInstance()->AddDebugLabel("Start GridPos X", 16, 160);
    InformationPanel::GetInstance()->AddDebugLabel("Start GridPos Y", 16, 180);
    InformationPanel::GetInstance()->AddDebugLabel("Start GridPos Z", 16, 200);
    InformationPanel::GetInstance()->AddDebugLabel("End GridPos X", 16, 220);
    InformationPanel::GetInstance()->AddDebugLabel("End GridPos Y", 16, 240);
    InformationPanel::GetInstance()->AddDebugLabel("End GridPos Z", 16, 260);
    InformationPanel::GetInstance()->AddDebugLabel("First Hit GridPos X", 16, 280);
    InformationPanel::GetInstance()->AddDebugLabel("First Hit GridPos Y", 16, 300);
    InformationPanel::GetInstance()->AddDebugLabel("First Hit GridPos Z", 16, 320);
    InformationPanel::GetInstance()->AddDebugLabel("Iteration Count", 16, 340);
#endif

    // Create GPU timer.
    mTimer = new GPUTimer();
    mTimer->CreateDeviceResource();
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::VoxelizeScene()
{
    mGround->SetCamera(mVoxelizationProjector);
    mCeiling->SetCamera(mVoxelizationProjector);
    mBackWall->SetCamera(mVoxelizationProjector);
    mLeftWall->SetCamera(mVoxelizationProjector);
    mRightWall->SetCamera(mVoxelizationProjector);
    mModel->SetCamera(mVoxelizationProjector);

    glViewport(0, 0, VOXEL_DIMENSION, VOXEL_DIMENSION);
	mGround->Render(0, 0);
	mCeiling->Render(0, 0);
	mBackWall->Render(0, 0);
	mLeftWall->Render(0, 0);
	mRightWall->Render(0, 0);

    glViewport(0, 0, (VOXEL_DIMENSION >> 4) + 4, (VOXEL_DIMENSION >> 4) + 4);
	mModel->Render(0, 0);
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::ShowVoxelization()
{
    mGround->SetCamera(mMainCamera);
    mCeiling->SetCamera(mMainCamera);
    mBackWall->SetCamera(mMainCamera);
    mLeftWall->SetCamera(mMainCamera);
    mRightWall->SetCamera(mMainCamera);
    mModel->SetCamera(mMainCamera);

    mGround->Render(0, 1);
    mCeiling->Render(0, 1);
    mBackWall->Render(0, 1);
    mLeftWall->Render(0, 1);
    mRightWall->Render(0, 1);
    mModel->Render(0, 1);
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::FrameFunc()
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

    InformationPanel^ infoPanel = InformationPanel::GetInstance();
    static double workLoad = 0.0;

    mVoxelBuffer->Bind(0);
    mIndirectCommandBuffer->Bind(1);
    mIndirectCommandBuffer->BindToIndirect();

    // Reset counter.
    mGatheredVoxelAllocCounter->Bind(0);
    mTimer->Start();
    GLuint* counterData = (GLuint*)mGatheredVoxelAllocCounter->Map(BA_Write_Only);
    assert(counterData);
    counterData[0] = 0;
    mGatheredVoxelAllocCounter->Unmap();
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Reset Counter Time", workLoad);

    // Reset voxel buffer pass.
    mTimer->Start();
    mResetVoxelBufferTask->DispatchCompute(0,
        VOXEL_DIMENSION / LOCAL_GROUP_DIM, 
        VOXEL_DIMENSION / LOCAL_GROUP_DIM, 
        VOXEL_DIMENSION / LOCAL_GROUP_DIM);
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Reset Voxel Buffer Pass", workLoad);

    // Debug reset voxel buffer task.
#ifdef DEBUG_VOXEL
    GLuint* bufferData = (GLuint*)mVoxelBuffer->Map(GL_WRITE_ONLY);
    assert(bufferData);
    int res = memcmp(mZeroBuffer, bufferData, VOXEL_DIMENSION*VOXEL_DIMENSION*VOXEL_DIMENSION*sizeof(GLuint));
    assert(res == 0);
    mVoxelBuffer->Unmap();
#endif

    // Scene voxelization pass.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    mTimer->Start();
	VoxelizeScene();
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Voxelization Pass", workLoad);

    // Gather voxel buffer pass.
    mGatherVoxelBufferTask->DispatchCompute(0,
        VOXEL_DIMENSION / LOCAL_GROUP_DIM, 
        VOXEL_DIMENSION / LOCAL_GROUP_DIM, 
        VOXEL_DIMENSION / LOCAL_GROUP_DIM);
#ifdef DEBUG_VOXEL
    GLuint* indirectCommandbufferData = (GLuint*)mIndirectCommandBuffer->Map(GL_READ_ONLY);
    GLfloat* gatheredVoxelData = (GLfloat*)(indirectCommandbufferData + 10);
    infoPanel->SetLabelValue("Voxel Count", (double)indirectCommandbufferData[1]);
    mIndirectCommandBuffer->Unmap();
#endif

    counterData = (GLuint*)mGatheredVoxelAllocCounter->Map(BA_Write_Only);
    infoPanel->SetTimingLabelValue("Counter", (double)counterData[0]);
    mGatheredVoxelAllocCounter->Unmap();

    // Visualize scene voxelization pass.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, Width, Height);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mTimer->Start();
    if( mShowMode == SM_VoxelGrid )
    {
        mVoxelCubeModel->Render(0, 0);
    }
    else
    {
        ShowVoxelization();
    }
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Visualization Pass", workLoad);

    if( mVoxelRaySegment )
    {
        mVoxelBuffer->Bind(0);
        mIndirectCommandBuffer->Bind(1);

        mTimer->Start();
        mVoxelGridIntersectionTask->DispatchCompute(0, 1, 1, 1);
        mTimer->Stop();
        workLoad = mTimer->GetTimeElapsed();
        infoPanel->SetTimingLabelValue("Intersection Pass", workLoad);

#ifdef DEBUG_VOXEL_RAY_INTERSECTION
        GLuint* indirectCommandbufferData = (GLuint*)mIndirectCommandBuffer->Map(GL_READ_ONLY);
        GLfloat* intersectionData = (GLfloat*)(indirectCommandbufferData + 8);

        InformationPanel::GetInstance()->SetDebugLabelValue("Ray Direction X", (double)intersectionData[1]);
        InformationPanel::GetInstance()->SetDebugLabelValue("Ray Direction Y", (double)intersectionData[2]);
        InformationPanel::GetInstance()->SetDebugLabelValue("Ray Direction Z", (double)intersectionData[3]);
        InformationPanel::GetInstance()->SetDebugLabelValue("Voxel Extension X", (double)intersectionData[4]);
        InformationPanel::GetInstance()->SetDebugLabelValue("Voxel Extension Y", (double)intersectionData[5]);
        InformationPanel::GetInstance()->SetDebugLabelValue("Voxel Extension Z", (double)intersectionData[6]);
        InformationPanel::GetInstance()->SetDebugLabelValue("Ray maxT", (double)intersectionData[7]);
        InformationPanel::GetInstance()->SetDebugLabelValue("Start GridPos X", (double)intersectionData[8]);
        InformationPanel::GetInstance()->SetDebugLabelValue("Start GridPos Y", (double)intersectionData[9]);
        InformationPanel::GetInstance()->SetDebugLabelValue("Start GridPos Z", (double)intersectionData[10]);
        InformationPanel::GetInstance()->SetDebugLabelValue("End GridPos X", (double)intersectionData[11]);
        InformationPanel::GetInstance()->SetDebugLabelValue("End GridPos Y", (double)intersectionData[12]);
        InformationPanel::GetInstance()->SetDebugLabelValue("End GridPos Z", (double)intersectionData[13]);
        InformationPanel::GetInstance()->SetDebugLabelValue("First Hit GridPos X", (double)intersectionData[14]);
        InformationPanel::GetInstance()->SetDebugLabelValue("First Hit GridPos Y", (double)intersectionData[15]);
        InformationPanel::GetInstance()->SetDebugLabelValue("First Hit GridPos Z", (double)intersectionData[16]);
        InformationPanel::GetInstance()->SetDebugLabelValue("Iteration Count", (double)intersectionData[17]);

        mIndirectCommandBuffer->Unmap();
#endif

        mVoxelRaySegment->Render(0, 0);
    }
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::Terminate()
{
	// Release all resources.
	delete mVoxelizationProjector;

    mGatheredVoxelAllocCounter = 0;
    mResetVoxelBufferTask = 0;
    mGatherVoxelBufferTask = 0;
    mVoxelGridIntersectionTask = 0;
    mVoxelBuffer = 0;
    mIndirectCommandBuffer = 0;

	mGround = 0;
	mCeiling = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;
    mVoxelCubeModel = 0;
    mVoxelRaySegment = 0;

    mTimer = 0;
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::ProcessInput()
{
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
		mShowMode = SM_VoxelGrid;
	}
	if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
	{
		mShowMode = SM_Scene;
	}
	if (glfwGetKey(Window, GLFW_KEY_3) == GLFW_PRESS)
	{
		mShowMode = SM_WorldPosition;
	}
	if (glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS)
	{
		mIsRotatingModel = !mIsRotatingModel;
	}
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::OnButtonClick(System::Object^  sender, 
    System::EventArgs^  e)
{
    String^ p1 = InformationPanel::GetInstance()->GetTextBoxValue("P1:");
    String^ p2 = InformationPanel::GetInstance()->GetTextBoxValue("P2:");

    array<String^, 1>^ p1Res = p1->Split(',');
    array<String^, 1>^ p2Res = p2->Split(',');
    for( int i = 0; i < 3; ++i )
    {
        RaySegment[i] = (float)Convert::ToDouble((String^)p1Res[i]);
        RaySegment[i + 3] = (float)Convert::ToDouble((String^)p2Res[i]);
    }

    ShaderProgramInfo voxelRaySegmentProgramInfo;
    voxelRaySegmentProgramInfo.VShaderFileName = "SimpleVoxelization/vVoxelRaySegment.glsl";
    voxelRaySegmentProgramInfo.FShaderFileName = "SimpleVoxelization/fVoxelRaySegment.glsl";
    voxelRaySegmentProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passVoxelRaySegment = new Pass(voxelRaySegmentProgramInfo);

    Technique* techVoxelRaySegment = new Technique();
    techVoxelRaySegment->AddPass(passVoxelRaySegment);
    MaterialTemplate* mtVoxelRaySegment = new MaterialTemplate();
    mtVoxelRaySegment->AddTechnique(techVoxelRaySegment);
    Material* material = new Material(mtVoxelRaySegment);
    mVoxelRaySegment = new VoxelRaySegment(material, mMainCamera);
    mVoxelRaySegment->LineWidth = 3.0f;
    std::vector<int> temp;
    temp.reserve(1);
    temp.push_back(2);
    mVoxelRaySegment->LoadFromMemory(1, temp, 2, RaySegment);
    mVoxelRaySegment->CreateDeviceResource(mDevice);
}
//----------------------------------------------------------------------------