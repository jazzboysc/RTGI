#include "SVOApp.h"
#include <glfw3.h>

using namespace RTGI;
using namespace RTGI::GUIFramework;

float SVOApp::RaySegment[6] = { 0.0f, 0.0f, 0.0f, 
                                0.0f, 0.0f, 0.0f };

//#define DEBUG_VOXEL
//#define DEBUG_VOXEL_RAY_INTERSECTION

//----------------------------------------------------------------------------
SVOApp::SVOApp(int width, int height)
{
	Width = width;
	Height = height;
	Title = "Sparse voxel octree demo";
    mIsRotatingModel = false;
    mVoxelizeCornell = false;
    mShowCornell = false;
    mShowMode = SM_VoxelGrid;
}
//----------------------------------------------------------------------------
SVOApp::~SVOApp()
{
}
//----------------------------------------------------------------------------
void SVOApp::Initialize(GPUDevice* device)
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
    mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width / (float)Height, 1.0f, 150.0f);
    mMainCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo voxelizationProgramInfo;
    voxelizationProgramInfo.VShaderFileName = "SparseVoxelOctree/vVoxelization.glsl";
    voxelizationProgramInfo.GShaderFileName = "SparseVoxelOctree/gVoxelization.glsl";
    voxelizationProgramInfo.FShaderFileName = "SparseVoxelOctree/fVoxelization.glsl";
    voxelizationProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                              ShaderType::ST_Geometry | 
                                              ShaderType::ST_Fragment;
    Pass* passVoxelization = new Pass(voxelizationProgramInfo);

    ShaderProgramInfo showVoxelizationProgramInfo;
    showVoxelizationProgramInfo.VShaderFileName = "SparseVoxelOctree/vShowVoxelization.glsl";
    showVoxelizationProgramInfo.FShaderFileName = "SparseVoxelOctree/fShowVoxelization.glsl";
    showVoxelizationProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                                  ShaderType::ST_Fragment;
    Pass* passShowVoxelization = new Pass(showVoxelizationProgramInfo);

    Technique* techVoxelization = new Technique();
    techVoxelization->AddPass(passVoxelization);
    techVoxelization->AddPass(passShowVoxelization);
    MaterialTemplate* mtVoxelization = new MaterialTemplate();
    mtVoxelization->AddTechnique(techVoxelization);

    ShaderProgramInfo showVoxelGridProgramInfo;
    showVoxelGridProgramInfo.VShaderFileName = "SparseVoxelOctree/vShowVoxelGrid.glsl";
    showVoxelGridProgramInfo.FShaderFileName = "SparseVoxelOctree/fShowVoxelGrid.glsl";
    showVoxelGridProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                               ShaderType::ST_Fragment;
    Pass* passShowVoxelGrid = new Pass(showVoxelGridProgramInfo);

    Technique* techShowVoxelGrid = new Technique();
    techShowVoxelGrid->AddPass(passShowVoxelGrid);
    MaterialTemplate* mtShowVoxelGrid = new MaterialTemplate();
    mtShowVoxelGrid->AddTechnique(techShowVoxelGrid);

    // Create reset voxel buffer task.
    ShaderProgramInfo resetVoxelBufferProgramInfo;
    resetVoxelBufferProgramInfo.CShaderFileName = "SparseVoxelOctree/cResetVoxelBuffer.glsl";
    resetVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passResetVoxelBuffer = new ComputePass(resetVoxelBufferProgramInfo);
    mResetVoxelBufferTask = new ResetVoxelBuffer();
    mResetVoxelBufferTask->AddPass(passResetVoxelBuffer);
    mResetVoxelBufferTask->CreateDeviceResource(mDevice);

    // Create gather voxel buffer task.
    ShaderProgramInfo gatherVoxelBufferProgramInfo;
    gatherVoxelBufferProgramInfo.CShaderFileName = "SparseVoxelOctree/cGatherVoxelBuffer.glsl";
    gatherVoxelBufferProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passGatherVoxelBuffer = new ComputePass(gatherVoxelBufferProgramInfo);
    mGatherVoxelBufferTask = new GatherVoxelBuffer();
    mGatherVoxelBufferTask->AddPass(passGatherVoxelBuffer);
    mGatherVoxelBufferTask->CreateDeviceResource(mDevice);
    mGatherVoxelBufferTask->SceneBB = &mSceneBB;

    // Create voxel grid intersection task.
    ShaderProgramInfo voxelGridIntersectionProgramInfo;
    voxelGridIntersectionProgramInfo.CShaderFileName = "SparseVoxelOctree/cVoxelGridIntersection.glsl";
    voxelGridIntersectionProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passVoxelGridIntersection = new ComputePass(voxelGridIntersectionProgramInfo);
    mVoxelGridIntersectionTask = new VoxelGridIntersection();
    mVoxelGridIntersectionTask->AddPass(passVoxelGridIntersection);
    mVoxelGridIntersectionTask->CreateDeviceResource(mDevice);
    mVoxelGridIntersectionTask->SceneBB = &mSceneBB;

    // Create gather voxel fragment list info task.
    ShaderProgramInfo gatherVoxelFragmentListInfoProgramInfo;
    gatherVoxelFragmentListInfoProgramInfo.CShaderFileName = "SparseVoxelOctree/cGatherVoxelFragmentListInfo.glsl";
    gatherVoxelFragmentListInfoProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;

    ComputePass* passGatherVoxelFragmentListInfo = new ComputePass(gatherVoxelFragmentListInfoProgramInfo);
    mGatherVoxelFragmentListInfoTask = new GatherVoxelFragmentListInfo();
    mGatherVoxelFragmentListInfoTask->AddPass(passGatherVoxelFragmentListInfo);
    mGatherVoxelFragmentListInfoTask->CreateDeviceResource(mDevice);

    // Create build SVO task.
    ShaderProgramInfo buildSVOInitRootProgramInfo;
    buildSVOInitRootProgramInfo.VShaderFileName = "SparseVoxelOctree/vBuildSVOInitRoot.glsl";
    buildSVOInitRootProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOInitRoot = new ComputePass(buildSVOInitRootProgramInfo);

    ShaderProgramInfo buildSVOFlagNodesProgramInfo;
    buildSVOFlagNodesProgramInfo.VShaderFileName = "SparseVoxelOctree/vBuildSVOFlagNodes.glsl";
    buildSVOFlagNodesProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOFlagNodes = new ComputePass(buildSVOFlagNodesProgramInfo);

    ShaderProgramInfo buildSVOAllocateNodesProgramInfo;
    buildSVOAllocateNodesProgramInfo.VShaderFileName = "SparseVoxelOctree/vBuildSVOAllocateNodes.glsl";
    buildSVOAllocateNodesProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOAllocateNodes = new ComputePass(buildSVOAllocateNodesProgramInfo);

    ShaderProgramInfo buildSVOPostAllocateNodesProgramInfo;
    buildSVOPostAllocateNodesProgramInfo.VShaderFileName = "SparseVoxelOctree/vBuildSVOPostAllocateNodes.glsl";
    buildSVOPostAllocateNodesProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOPostAllocateNodes = new ComputePass(buildSVOPostAllocateNodesProgramInfo);

    ShaderProgramInfo buildSVOInitNodesProgramInfo;
    buildSVOInitNodesProgramInfo.VShaderFileName = "SparseVoxelOctree/vBuildSVOInitNodes.glsl";
    buildSVOInitNodesProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOInitNodes = new ComputePass(buildSVOInitNodesProgramInfo);

    mBuildSVOTask = new BuildSVO();
    mBuildSVOTask->AddPass(passBuildSVOInitRoot);
    mBuildSVOTask->AddPass(passBuildSVOFlagNodes);
    mBuildSVOTask->AddPass(passBuildSVOAllocateNodes);
    mBuildSVOTask->AddPass(passBuildSVOPostAllocateNodes);
    mBuildSVOTask->AddPass(passBuildSVOInitNodes);
    mBuildSVOTask->CreateDeviceResource(mDevice);

    // Create scene voxel buffer.
    mVoxelBuffer = new StructuredBuffer();
    GLuint voxelCount = VOXEL_DIMENSION * VOXEL_DIMENSION * VOXEL_DIMENSION;
    GLuint bufferSize = voxelCount * sizeof(GLuint) * 4;
    mVoxelBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Copy);

    // Create indirect command buffer.
    mIndirectCommandBuffer = new StructuredBuffer();
    bufferSize = sizeof(GLuint)*5 + sizeof(GLfloat)*35 + voxelCount*sizeof(GLfloat)*4;
    mIndirectCommandBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Copy);

    // Create voxel fragment list buffer.
    mVoxelFragmentListBuffer = new StructuredBuffer();
    GLuint voxelFragmentCount = GLuint((float)voxelCount*0.2f); // voxel fragment ratio.
    bufferSize = sizeof(GLuint)*4 + voxelFragmentCount*sizeof(GLuint)*4;
    mVoxelFragmentListBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Copy);

    // Create SVO buffer.
    mSVOMaxLevel = (unsigned int)glm::log2((float)VOXEL_DIMENSION);
    mSVOBuffer = new StructuredBuffer();
    mSVONodeCount = unsigned int((float)voxelCount*(0.143f + 0.05f)); // interior node ratio (1/7, fixed) and leaf node ratio.
    bufferSize = sizeof(SVONodeBufferHead) + mSVONodeCount*sizeof(SVONode);
    mSVOBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Copy);

    // Create SVO uniform buffer.
    mSVOUniformBuffer = new UniformBuffer();
    bufferSize = sizeof(GLuint)*2;
    mSVOUniformBuffer->ReserveMutableDeviceResource(mDevice, bufferSize, BU_Dynamic_Draw);
    GLuint svoUniformBufferData[2] = { 0, VOXEL_DIMENSION };
    mSVOUniformBuffer->UpdateSubData(0, 0, sizeof(GLuint)*2, (void*)svoUniformBufferData);

    // Create atomic counter buffer. We create 4 atomic counters here.
    mAtomicCounterBuffer = new AtomicCounterBuffer();
#ifdef DEBUG_VOXEL
    mAtomicCounterBuffer->ReserveMutableDeviceResource(mDevice, sizeof(GLuint)*4, BU_Dynamic_Copy);
#else
    mAtomicCounterBuffer->ReserveImmutableDeviceResource(mDevice, sizeof(GLuint)*4);
#endif

	// Create scene.
	mat4 rotM;
    material = new Material(mtVoxelization);
	mModel = new SVOTriMesh(material, mMainCamera);
	mModel->LoadFromFile("dragon_s.ply");
    mat4 scale = glm::scale(mat4(), vec3(60.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource(mDevice);
    mModel->SetWorldTranslation(vec3(0.0f, 4.4f, 3.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);
    mModel->SceneBB = &mSceneBB;
    mSceneBB.Merge(mModel->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
    mGround = new SVOTriMesh(material, mMainCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource(mDevice);
    mGround->MaterialColor = vec3(0.8f, 0.8f, 0.0f);
    mGround->SceneBB = &mSceneBB;
    mSceneBB.Merge(mGround->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
    mCeiling = new SVOTriMesh(material, mMainCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource(mDevice);
	rotM = rotate(mat4(), radians(180.0f), vec3(1, 0, 0));
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);
    mCeiling->SceneBB = &mSceneBB;
    mSceneBB.Merge(mCeiling->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
    mBackWall = new SVOTriMesh(material, mMainCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);
    mBackWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mBackWall->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
    mLeftWall = new SVOTriMesh(material, mMainCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(-90.0f), vec3(0, 0, 1));
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);
    mLeftWall->SceneBB = &mSceneBB;
    mSceneBB.Merge(mLeftWall->GetWorldSpaceBB());

    material = new Material(mtVoxelization);
    mRightWall = new SVOTriMesh(material, mMainCamera);
	mRightWall->LoadFromFile("square.ply");
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
    mVoxelCubeModel->LoadFromFile("box.ply");
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
    InformationPanel::GetInstance()->AddTimingLabel("Voxel Fragment Count", 16, 160);
    InformationPanel::GetInstance()->AddTimingLabel("GVF Count", 16, 180);
    InformationPanel::GetInstance()->AddTimingLabel("Build SVO Init Root Pass", 16, 200);
    InformationPanel::GetInstance()->AddTimingLabel("Build SVO Pass", 16, 220);
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

#ifdef DEBUG_VOXEL
    InformationPanel::GetInstance()->AddTimingLabel("Voxel Ratio", 16, 280);
    InformationPanel::GetInstance()->AddTimingLabel("Voxel Fragment Ratio", 16, 300);
#endif

    // Create GPU timer.
    mTimer = new GPUTimer();
    mTimer->CreateDeviceResource();
}
//----------------------------------------------------------------------------
void SVOApp::VoxelizeScene()
{
    if( mVoxelizeCornell )
    {
        glViewport(0, 0, VOXEL_DIMENSION, VOXEL_DIMENSION);
        mGround->Render(0, 0);
        mCeiling->Render(0, 0);
        mBackWall->Render(0, 0);
        mLeftWall->Render(0, 0);
        mRightWall->Render(0, 0);
    }

    glViewport(0, 0, 2, 2);
	mModel->Render(0, 0);
}
//----------------------------------------------------------------------------
void SVOApp::ShowVoxelization()
{
    if( mVoxelizeCornell && mShowCornell )
    {
        mGround->Render(0, 1);
        mCeiling->Render(0, 1);
        mBackWall->Render(0, 1);
        mLeftWall->Render(0, 1);
        mRightWall->Render(0, 1);
    }

    mModel->Render(0, 1);
}
//----------------------------------------------------------------------------
void SVOApp::FrameFunc()
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
    mIndirectCommandBuffer->Bind(2);

    // Reset counters.
    mAtomicCounterBuffer->Bind(0);
    mTimer->Start();
#ifdef DEBUG_VOXEL
    GLuint* counterData = (GLuint*)mAtomicCounterBuffer->Map(BA_Write_Only);
    assert(counterData);
    counterData[0] = 0;
    counterData[1] = 0;
    counterData[2] = 0;
    counterData[3] = 0;
    mAtomicCounterBuffer->Unmap();
#else
    unsigned int counterData[4];
    counterData[0] = 0;
    counterData[1] = 0;
    counterData[2] = 0;
    counterData[3] = 0;
    mAtomicCounterBuffer->Clear(BIF_R32UI, BF_R32UI, BCT_Unsigned_Int, counterData);
#endif
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

    // Scene voxelization pass.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    mTimer->Start();
    mVoxelFragmentListBuffer->Bind(1);
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
    mIndirectCommandBuffer->Bind();
    GLuint* indirectCommandbufferData = (GLuint*)mIndirectCommandBuffer->Map(BA_Read_Only);
    GLfloat* gatheredVoxelData = (GLfloat*)(indirectCommandbufferData + 10);
    infoPanel->SetTimingLabelValue("Voxel Count", (double)indirectCommandbufferData[1]);
    mIndirectCommandBuffer->Unmap();

    mAtomicCounterBuffer->Bind();
    counterData = (GLuint*)mAtomicCounterBuffer->Map(BA_Write_Only);
    infoPanel->SetTimingLabelValue("Voxel Fragment Count", (double)counterData[0]);
    infoPanel->SetTimingLabelValue("Counter", (double)counterData[3]);
    mAtomicCounterBuffer->Unmap();

    float voxelRatio = (float)counterData[3] / (float)(VOXEL_DIMENSION*VOXEL_DIMENSION*VOXEL_DIMENSION);
    float voxelFragmentRatio = (float)counterData[0] / (float)(VOXEL_DIMENSION*VOXEL_DIMENSION*VOXEL_DIMENSION);
    infoPanel->SetTimingLabelValue("Voxel Ratio", (double)voxelRatio);
    infoPanel->SetTimingLabelValue("Voxel Fragment Ratio", (double)voxelFragmentRatio);
#endif

    // Gather voxel fragment list info pass.
    mVoxelFragmentListBuffer->Bind(1);
    mGatherVoxelFragmentListInfoTask->DispatchCompute(0, 1, 1, 1);
#ifdef DEBUG_VOXEL
    mVoxelFragmentListBuffer->Bind();
    GLuint* dispatchIndirectCommandbufferData = (GLuint*)mVoxelFragmentListBuffer->Map(BA_Read_Only);
    infoPanel->SetTimingLabelValue("GVF Count", (double)dispatchIndirectCommandbufferData[0]);
    mVoxelFragmentListBuffer->Unmap();
#endif

    // Build SVO init root pass.
    mSVOUniformBuffer->Bind(0);
    mVoxelFragmentListBuffer->Bind(1);
    mSVOBuffer->Bind(3);
    mTimer->Start();
    mBuildSVOTask->DispatchVertex(BUILD_SVO_INIT_ROOT_PASS, 1);
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Build SVO Init Root Pass", workLoad);
#ifdef DEBUG_VOXEL
    mSVOBuffer->Bind();
    void* svoBufferData = mSVOBuffer->Map(BA_Read_Only);
    SVONodeBufferHead* svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
    SVONodeTile* svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
    mSVOBuffer->Unmap();
#endif

    mTimer->Start();
    unsigned int curLevel = 1;
    for( ; curLevel <= mSVOMaxLevel; ++curLevel )
    {
        // Update SVO uniform buffer.
        mSVOUniformBuffer->UpdateSubData(0, 0, sizeof(unsigned int), (void*)&curLevel);
#ifdef DEBUG_VOXEL
        GLuint* svoUniformBufferData = (GLuint*)mSVOUniformBuffer->Map(BA_Read_Only);
        mSVOUniformBuffer->Unmap();
#endif

        // Flag SVO nodes pass.
        mVoxelFragmentListBuffer->Bind(1);
        mSVOBuffer->Bind(3);
        mBuildSVOTask->DispatchVertexIndirect(BUILD_SVO_FLAG_NODES_PASS, 
            mVoxelFragmentListBuffer, 0);
#ifdef DEBUG_VOXEL
        mSVOBuffer->Bind();
        svoBufferData = mSVOBuffer->Map(BA_Read_Only);
        svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
        svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
        mSVOBuffer->Unmap();
#endif

        // Allocate SVO nodes pass.
        mBuildSVOTask->DispatchVertexIndirect(BUILD_SVO_ALLOC_NODES_PASS, 
            mSVOBuffer, 0);
#ifdef DEBUG_VOXEL
        mSVOBuffer->Bind();
        svoBufferData = mSVOBuffer->Map(BA_Read_Only);
        svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
        svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
        mSVOBuffer->Unmap();
#endif

        // Post allocate SVO nodes pass.
        mBuildSVOTask->DispatchVertex(BUILD_SVO_POST_ALLOC_NODES_PASS, 1);

        // Init SVO nodes pass.
        mBuildSVOTask->DispatchVertexIndirect(BUILD_SVO_INIT_NODES_PASS, 
            mSVOBuffer, 0);
#ifdef DEBUG_VOXEL
        mSVOBuffer->Bind();
        svoBufferData = mSVOBuffer->Map(BA_Read_Only);
        svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
        svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
        mSVOBuffer->Unmap();
#endif
    }
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Build SVO Pass", workLoad);

    // Visualize scene voxelization pass.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, Width, Height);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mTimer->Start();
    if( mShowMode == SM_VoxelGrid )
    {
        mIndirectCommandBuffer->BindToIndirect();
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
        mIndirectCommandBuffer->Bind(2);

        mTimer->Start();
        mVoxelGridIntersectionTask->DispatchCompute(0, 1, 1, 1);
        mTimer->Stop();
        workLoad = mTimer->GetTimeElapsed();
        infoPanel->SetTimingLabelValue("Intersection Pass", workLoad);

#ifdef DEBUG_VOXEL_RAY_INTERSECTION
        mIndirectCommandBuffer->Bind();
        GLuint* indirectCommandbufferData = (GLuint*)mIndirectCommandBuffer->Map(BA_Read_Only);
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
void SVOApp::Terminate()
{
	// Release all resources.

    mAtomicCounterBuffer = 0;
    mResetVoxelBufferTask = 0;
    mGatherVoxelBufferTask = 0;
    mVoxelGridIntersectionTask = 0;
    mGatherVoxelFragmentListInfoTask = 0;
    mBuildSVOTask = 0;
    mVoxelBuffer = 0;
    mIndirectCommandBuffer = 0;
    mVoxelFragmentListBuffer = 0;
    mSVOBuffer = 0;
    mSVOUniformBuffer = 0;

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
void SVOApp::ProcessInput()
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
    if( glfwGetKey(Window, GLFW_KEY_T) == GLFW_PRESS )
    {
        mVoxelizeCornell = !mVoxelizeCornell;
    }
    if( glfwGetKey(Window, GLFW_KEY_Y) == GLFW_PRESS )
    {
        mShowCornell = !mShowCornell;
    }
}
//----------------------------------------------------------------------------
void SVOApp::OnButtonClick(System::Object^  sender, 
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
    voxelRaySegmentProgramInfo.VShaderFileName = "SparseVoxelOctree/vVoxelRaySegment.glsl";
    voxelRaySegmentProgramInfo.FShaderFileName = "SparseVoxelOctree/fVoxelRaySegment.glsl";
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