#include "SimpleVoxelizationApp.h"
#include "RNG.h"

using namespace RTGI;
using namespace RTGI::GUIFramework;

float SimpleVoxelizationApp::RaySegment[6] = { 0.0f, 0.0f, 0.0f, 
                                               0.0f, 0.0f, 0.0f };

//----------------------------------------------------------------------------
SimpleVoxelizationApp::SimpleVoxelizationApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("Simple voxelization demo")
{
    mIsRotatingModel = false;
    mShowMode = SM_VoxelGrid;
}
//----------------------------------------------------------------------------
SimpleVoxelizationApp::~SimpleVoxelizationApp()
{
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::Initialize()
{
    GLint globalX, globalY, globalZ;
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &globalX);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &globalY);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &globalZ);
    assert(globalX >= VOXEL_DIMENSION / LOCAL_GROUP_DIM &&
           globalY >= VOXEL_DIMENSION / LOCAL_GROUP_DIM &&
           globalZ >= VOXEL_DIMENSION / LOCAL_GROUP_DIM);

	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   
    // Create scene camera.
    mCamera = new Camera();
    mCamera->SetPerspectiveFrustum(45.0f, (float)mWidth / (float)mHeight, 1.0f, 50.0f);
    mCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

    // Create voxelization projector.
	mVoxelizationProjector = new Camera(false);
    mVoxelizationProjector->SetOrthogonalFrustum(10.5f, 1.0f, 0.01f, 20.5f);

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo voxelizationProgramInfo;
    voxelizationProgramInfo.VShaderFileName = "vVoxelization.glsl";
    voxelizationProgramInfo.GShaderFileName = "gVoxelization.glsl";
    voxelizationProgramInfo.FShaderFileName = "fVoxelization.glsl";
    voxelizationProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                              ShaderType::ST_Geometry | 
                                              ShaderType::ST_Fragment;
    Pass* passVoxelization = new Pass(voxelizationProgramInfo);

    ShaderProgramInfo showVoxelizationProgramInfo;
    showVoxelizationProgramInfo.VShaderFileName = "vShowVoxelization.glsl";
    showVoxelizationProgramInfo.FShaderFileName = "fShowVoxelization.glsl";
    showVoxelizationProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                                  ShaderType::ST_Fragment;
    Pass* passShowVoxelization = new Pass(showVoxelizationProgramInfo);

    Technique* techVoxelization = new Technique();
    techVoxelization->AddPass(passVoxelization);
    techVoxelization->AddPass(passShowVoxelization);
    MaterialTemplate* mtVoxelization = new MaterialTemplate();
    mtVoxelization->AddTechnique(techVoxelization);

    ShaderProgramInfo showVoxelGridProgramInfo;
    showVoxelGridProgramInfo.VShaderFileName = "vShowVoxelGrid.glsl";
    showVoxelGridProgramInfo.FShaderFileName = "fShowVoxelGrid.glsl";
    showVoxelGridProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                               ShaderType::ST_Fragment;
    Pass* passShowVoxelGrid = new Pass(showVoxelGridProgramInfo);

    Technique* techShowVoxelGrid = new Technique();
    techShowVoxelGrid->AddPass(passShowVoxelGrid);
    MaterialTemplate* mtShowVoxelGrid = new MaterialTemplate();
    mtShowVoxelGrid->AddTechnique(techShowVoxelGrid);

    // Create reset voxel buffer task.
    ShaderProgramInfo resetVoxelBufferProgramInfo;
    resetVoxelBufferProgramInfo.CShaderFileName = "cResetVoxelBuffer.glsl";
    resetVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passResetVoxelBuffer = new ComputePass(resetVoxelBufferProgramInfo);
    mResetVoxelBufferTask = new ResetVoxelBuffer();
    mResetVoxelBufferTask->AddPass(passResetVoxelBuffer);
    mResetVoxelBufferTask->CreateDeviceResource();

    // Create gather voxel buffer task.
    ShaderProgramInfo gatherVoxelBufferProgramInfo;
    gatherVoxelBufferProgramInfo.CShaderFileName = "cGatherVoxelBuffer.glsl";
    gatherVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passGatherVoxelBuffer = new ComputePass(gatherVoxelBufferProgramInfo);
    mGatherVoxelBufferTask = new GatherVoxelBuffer();
    mGatherVoxelBufferTask->AddPass(passGatherVoxelBuffer);
    mGatherVoxelBufferTask->CreateDeviceResource();
    mGatherVoxelBufferTask->SceneBB = &mSceneBB;

    // Create voxel grid intersection task.
    ShaderProgramInfo voxelGridIntersectionProgramInfo;
    voxelGridIntersectionProgramInfo.CShaderFileName = "cVoxelGridIntersection.glsl";
    voxelGridIntersectionProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passVoxelGridIntersection = new ComputePass(voxelGridIntersectionProgramInfo);
    mVoxelGridIntersectionTask = new VoxelGridIntersection();
    mVoxelGridIntersectionTask->AddPass(passVoxelGridIntersection);
    mVoxelGridIntersectionTask->CreateDeviceResource();
    mVoxelGridIntersectionTask->SceneBB = &mSceneBB;

    // Create scene voxel buffer.
    mVoxelBuffer = new StructuredBuffer();
    GLuint voxelCount = VOXEL_DIMENSION * VOXEL_DIMENSION * VOXEL_DIMENSION;
    GLuint bufferSize = sizeof(int) + voxelCount * sizeof(GLuint) * 4;
    mVoxelBuffer->ReserveDeviceResource(bufferSize, GL_DYNAMIC_COPY);
    memset(mZeroBuffer, 0x00, bufferSize);

    // Create indirect command buffer.
    mIndirectCommandBuffer = new StructuredBuffer();
    bufferSize = sizeof(GLuint)*5 + sizeof(GLfloat)*3 + voxelCount*sizeof(GLfloat)*4;
    mIndirectCommandBuffer->ReserveDeviceResource(bufferSize, GL_DYNAMIC_COPY);

    // Create gathered voxel GPU memory allocator counter.
    mGatheredVoxelAllocCounter = new AtomicCounterBuffer();
    mGatheredVoxelAllocCounter->ReserveDeviceResource(sizeof(GLuint), GL_DYNAMIC_COPY);

	// Create scene.
	mat4 rotM;
    material = new Material(mtVoxelization);
	mModel = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mModel->LoadFromFile("dragon_s.ply");
    mat4 scale = Scale(vec3(60.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
    mModel->SetWorldTranslation(vec3(0.0f, 4.4f, 3.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);
    mModel->SceneBB = &mSceneBB;
    mSceneBB.Merge(mModel->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
	mGround = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(0.8f, 0.8f, 0.0f);
    mGround->SceneBB = &mSceneBB;
    mSceneBB.Merge(mGround->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
	mCeiling = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);
    mCeiling->SceneBB = &mSceneBB;
    mSceneBB.Merge(mCeiling->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
	mBackWall = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    mBackWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mBackWall->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
	mLeftWall = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
    mLeftWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mLeftWall->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
	mRightWall = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource();
	rotM = RotateZ(90.0f);
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);
    mRightWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mRightWall->GetWorldSpaceBB());

    // Create voxel cube model.
    material = new Material(mtShowVoxelGrid);
    mVoxelCubeModel = new VoxelCubeTriMesh(material, mCamera);
    mVoxelCubeModel->LoadFromFile("box.ply");
    mVoxelCubeModel->GenerateNormals();
    mVoxelCubeModel->IsIndirect = true;
    mVoxelCubeModel->IndirectCommandBuffer = mIndirectCommandBuffer;
    mVoxelCubeModel->CreateDeviceResource();

    // Create GUI elements.
    InformationPanel::GetInstance()->AddListener(this);
    InformationPanel::GetInstance()->AddLabel("Reset Voxel Buffer Pass", 16, 20);
    InformationPanel::GetInstance()->AddLabel("Voxelization Pass", 16, 40);
    InformationPanel::GetInstance()->AddLabel("Voxel Count", 16, 60);
    InformationPanel::GetInstance()->AddLabel("Reset Counter Time", 16, 80);
    InformationPanel::GetInstance()->AddLabel("Counter", 16, 100);
    InformationPanel::GetInstance()->AddLabel("Visualization Pass", 16, 120);
    InformationPanel::GetInstance()->AddTextBox("P1:", 16, 20, 120, 16);
    InformationPanel::GetInstance()->AddTextBox("P2:", 16, 44, 120, 16);
    InformationPanel::GetInstance()->AddButton("Create Ray", 60, 80, 80, 24);

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
    mGround->SetCamera(mCamera);
    mCeiling->SetCamera(mCamera);
    mBackWall->SetCamera(mCamera);
    mLeftWall->SetCamera(mCamera);
    mRightWall->SetCamera(mCamera);
    mModel->SetCamera(mCamera);

    mGround->Render(0, 1);
    mCeiling->Render(0, 1);
    mBackWall->Render(0, 1);
    mLeftWall->Render(0, 1);
    mRightWall->Render(0, 1);
    mModel->Render(0, 1);
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::Run()
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

    InformationPanel^ infoPanel = InformationPanel::GetInstance();
    static double workLoad = 0.0;

    mVoxelBuffer->Bind(0);
    mIndirectCommandBuffer->Bind(1);
    mIndirectCommandBuffer->BindToIndirect();

    // Reset counter.
    mGatheredVoxelAllocCounter->Bind(0);
    mTimer->Start();
    GLuint* counterData = (GLuint*)mGatheredVoxelAllocCounter->Map(GL_WRITE_ONLY);
    assert(counterData);
    counterData[0] = 0;
    mGatheredVoxelAllocCounter->Unmap();
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetLabelValue("Reset Counter Time", workLoad);

    // Reset voxel buffer pass.
    mTimer->Start();
    mResetVoxelBufferTask->Dispatch(0, 
        VOXEL_DIMENSION / LOCAL_GROUP_DIM, 
        VOXEL_DIMENSION / LOCAL_GROUP_DIM, 
        VOXEL_DIMENSION / LOCAL_GROUP_DIM);
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetLabelValue("Reset Voxel Buffer Pass", workLoad);

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
    infoPanel->SetLabelValue("Voxelization Pass", workLoad);

    // Gather voxel buffer pass.
    mGatherVoxelBufferTask->Dispatch(0, 
        VOXEL_DIMENSION / LOCAL_GROUP_DIM, 
        VOXEL_DIMENSION / LOCAL_GROUP_DIM, 
        VOXEL_DIMENSION / LOCAL_GROUP_DIM);
#ifdef DEBUG_VOXEL
    GLuint* indirectCommandbufferData = (GLuint*)mIndirectCommandBuffer->Map(GL_READ_ONLY);
    GLfloat* gatheredVoxelData = (GLfloat*)(indirectCommandbufferData + 8);
    infoPanel->SetLabelValue("Voxel Count", (double)indirectCommandbufferData[1]);
    mIndirectCommandBuffer->Unmap();
#endif

    counterData = (GLuint*)mGatheredVoxelAllocCounter->Map(GL_WRITE_ONLY);
    infoPanel->SetLabelValue("Counter", (double)counterData[0]);
    mGatheredVoxelAllocCounter->Unmap();

    // Visualize scene voxelization pass.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, mWidth, mHeight);
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
    infoPanel->SetLabelValue("Visualization Pass", workLoad);

    if( mVoxelRaySegment )
    {
        mVoxelRaySegment->Render(0, 0);
        mVoxelGridIntersectionTask->Dispatch(0, 1, 1, 1);

        GLuint* testRes = (GLuint*)mVoxelBuffer->Map(GL_READ_ONLY);
        mVoxelBuffer->Unmap();
    }

	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::Terminate()
{
	// Release all resources.

    delete mCamera;
	delete mVoxelizationProjector;

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
void SimpleVoxelizationApp::OnKeyboard(unsigned char key, int x, int y)
{
    switch( key )
    {
    case '1':
        mShowMode = SM_VoxelGrid;
        break;

    case '2':
        mShowMode = SM_Scene;
        break;

    case '3':
        mShowMode = SM_WorldPosition;
        break;

    case 'r':
        mIsRotatingModel = !mIsRotatingModel;
        break;

    default:
        break;
    }
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::OnReshape(int x, int y)
{
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
    voxelRaySegmentProgramInfo.VShaderFileName = "vVoxelRaySegment.glsl";
    voxelRaySegmentProgramInfo.FShaderFileName = "fVoxelRaySegment.glsl";
    voxelRaySegmentProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
        ShaderType::ST_Fragment;
    Pass* passVoxelRaySegment = new Pass(voxelRaySegmentProgramInfo);

    Technique* techVoxelRaySegment = new Technique();
    techVoxelRaySegment->AddPass(passVoxelRaySegment);
    MaterialTemplate* mtVoxelRaySegment = new MaterialTemplate();
    mtVoxelRaySegment->AddTechnique(techVoxelRaySegment);
    Material* material = new Material(mtVoxelRaySegment);
    mVoxelRaySegment = new VoxelRaySegment(material, mCamera);
    mVoxelRaySegment->LineWidth = 3.0f;
    std::vector<int> temp;
    temp.reserve(1);
    temp.push_back(2);
    mVoxelRaySegment->LoadFromMemory(1, temp, 2, RaySegment);
    mVoxelRaySegment->CreateDeviceResource();
}
//----------------------------------------------------------------------------