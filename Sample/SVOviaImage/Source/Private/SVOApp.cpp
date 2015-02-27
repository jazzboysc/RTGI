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
	Title = "SVO via image demo";
    mIsRotatingModel = false;
    mVoxelizeCornell = false;
    mShowCornell = false;
    mShowSVOBoxBV = true;
    mShowMode = SM_WorldPosition;
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
    voxelizationProgramInfo.FShaderFileName = "SparseVoxelOctree/fSVOVoxelization.glsl";
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

    ShaderProgramInfo showSVOProgramInfo;
    showSVOProgramInfo.VShaderFileName = "SparseVoxelOctree/vShowSVO.glsl";
    showSVOProgramInfo.FShaderFileName = "SparseVoxelOctree/fShowSVO.glsl";
    showSVOProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                         ShaderType::ST_Fragment;
    Pass* passShowSVO = new Pass(showSVOProgramInfo);

    Technique* techShowSVO = new Technique();
    techShowSVO->AddPass(passShowSVO);
    MaterialTemplate* mtShowSVO = new MaterialTemplate();
    mtShowSVO->AddTechnique(techShowSVO);

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

    ShaderProgramInfo buildSVOSplatLeafNodesProgramInfo;
    buildSVOSplatLeafNodesProgramInfo.VShaderFileName = "SparseVoxelOctree/vBuildSVOSplatLeafNodes.glsl";
    buildSVOSplatLeafNodesProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex;
    ComputePass* passBuildSVOSplatLeafNodes = new ComputePass(buildSVOSplatLeafNodesProgramInfo);

    mBuildSVOTask = new BuildSVO();
    mBuildSVOTask->AddPass(passBuildSVOInitRoot);
    mBuildSVOTask->AddPass(passBuildSVOFlagNodes);
    mBuildSVOTask->AddPass(passBuildSVOAllocateNodes);
    mBuildSVOTask->AddPass(passBuildSVOPostAllocateNodes);
    mBuildSVOTask->AddPass(passBuildSVOInitNodes);
    mBuildSVOTask->AddPass(passBuildSVOSplatLeafNodes);
    mBuildSVOTask->CreateDeviceResource(mDevice);

    // Create SVO ray intersection task.
    ShaderProgramInfo svoRayIntersectionProgramInfo;
    svoRayIntersectionProgramInfo.CShaderFileName = "SparseVoxelOctree/cSVORayIntersection.glsl";
    svoRayIntersectionProgramInfo.ShaderStageFlag = ShaderType::ST_Compute;
    ComputePass* passSVORayIntersection = new ComputePass(svoRayIntersectionProgramInfo);

    mSVORayIntersectionTask = new SVORayIntersection();
    mSVORayIntersectionTask->AddPass(passSVORayIntersection);
    mSVORayIntersectionTask->CreateDeviceResource(mDevice);

    // Create voxel fragment list buffer.
    GLuint voxelCount = VOXEL_DIMENSION * VOXEL_DIMENSION * VOXEL_DIMENSION;
    mVoxelFragmentListBuffer = new StructuredBuffer();
    GLuint voxelFragmentCount = GLuint((float)voxelCount*0.2f); // voxel fragment ratio.
    GLuint bufferSize = sizeof(VoxelFragmentBufferHead) + voxelFragmentCount*sizeof(VoxelFragment);
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

    // Create atomic counter buffer. We create 2 atomic counters here.
    mAtomicCounterBuffer = new AtomicCounterBuffer();
#ifdef DEBUG_VOXEL
    mAtomicCounterBuffer->ReserveMutableDeviceResource(mDevice, sizeof(GLuint)*2, BU_Dynamic_Copy);
#else
    mAtomicCounterBuffer->ReserveImmutableDeviceResource(mDevice, sizeof(GLuint)*2);
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

    // Initialize scene bounding box buffer.
    vec3 sceneBBMin = mSceneBB.Min;
    vec3 center = mSceneBB.GetBoxCenter();
    vec3 extension = mSceneBB.GetExtension();
    vec3 inv2extension = vec3(1.0f / (2.0f*extension.x), 1.0f / (2.0f*extension.y), 1.0f / (2.0f*extension.z));
    mVoxelFragmentListBuffer->Bind();
    VoxelFragmentBufferHead* bufferData = (VoxelFragmentBufferHead*)mVoxelFragmentListBuffer->Map(BA_Write_Only);
    bufferData->SceneBBMin = vec4(sceneBBMin, 1.0);
    bufferData->SceneBBCenter = vec4(center, 1.0);
    bufferData->SceneBBExtension = vec4(extension, 0.0);
    bufferData->Inv2SceneBBExtension = vec4(inv2extension, 0.0);
    mVoxelFragmentListBuffer->Unmap();

    // Create SVO node cube model.
    std::vector<vec3> svoCubeVertices;
    svoCubeVertices.reserve(8);
    svoCubeVertices.push_back(vec3(-1.0f,  1.0f,  1.0f));
    svoCubeVertices.push_back(vec3( 1.0f,  1.0f,  1.0f));
    svoCubeVertices.push_back(vec3( 1.0f,  1.0f, -1.0f));
    svoCubeVertices.push_back(vec3(-1.0f,  1.0f, -1.0f));
    svoCubeVertices.push_back(vec3(-1.0f, -1.0f,  1.0f));
    svoCubeVertices.push_back(vec3( 1.0f, -1.0f,  1.0f));
    svoCubeVertices.push_back(vec3( 1.0f, -1.0f, -1.0f));
    svoCubeVertices.push_back(vec3(-1.0f, -1.0f, -1.0f));
    std::vector<unsigned short> svoCubeIndices;
    svoCubeIndices.reserve(24);
    static unsigned short indices[] = { 0, 1, 2, 3,
                                        0, 4, 5, 1,
                                        0, 3, 7, 4,
                                        2, 6, 7, 3,
                                        1, 5, 6, 2,
                                        4, 7, 6, 5 };
    for( int i = 0; i < 24; ++i )
    {
        svoCubeIndices.push_back(indices[i]);
    }

    material = new Material(mtShowSVO);
    mSVONodeCubeModel = new SVOCubeMesh(material, mMainCamera);
    mSVONodeCubeModel->LoadFromSystemMemory(svoCubeVertices, svoCubeIndices);
    mSVONodeCubeModel->SetIndirectCommandBuffer(mSVOBuffer, 16);
    mSVONodeCubeModel->CreateDeviceResource(mDevice);
    mSVONodeCubeModel->SceneBB = &mSceneBB;

	// Create information panel.
	int screenX, screenY;
	glfwGetWindowPos(Window, &screenX, &screenY);
	InformationPanel^ infoPanel = gcnew InformationPanel();
	infoPanel->Show();
	infoPanel->SetDesktopLocation(screenX + Width + 12, screenY - 30);

    // Create GUI elements.
    InformationPanel::GetInstance()->AddListener(this);
    InformationPanel::GetInstance()->AddTimingLabel("Voxelization Pass", 16, 20);
    InformationPanel::GetInstance()->AddTimingLabel("Reset Counter Time", 16, 40);
    InformationPanel::GetInstance()->AddTimingLabel("Visualization Pass", 16, 60);
    InformationPanel::GetInstance()->AddTimingLabel("Voxel Fragment Count", 16, 80);
    InformationPanel::GetInstance()->AddTimingLabel("GVF Count", 16, 100);
    InformationPanel::GetInstance()->AddTimingLabel("Build SVO Init Root Pass", 16, 120);
    InformationPanel::GetInstance()->AddTimingLabel("Build SVO Pass", 16, 140);
    InformationPanel::GetInstance()->AddTimingLabel("Build SVO Splat Leaf Nodes Pass", 16, 160);
    InformationPanel::GetInstance()->AddTimingLabel("SVO Ray Intersection Pass", 16, 180);
    InformationPanel::GetInstance()->AddTextBox("P1:", 16, 20, 120, 16);
    InformationPanel::GetInstance()->AddTextBox("P2:", 16, 44, 120, 16);
    InformationPanel::GetInstance()->AddButton("Create Ray", 60, 80, 80, 24);
    InformationPanel::GetInstance()->AddCheckBox("Show SVO Node BV", 16, 120, 60, 20, true);

#ifdef DEBUG_VOXEL_RAY_INTERSECTION
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

    // Reset counters.
    mAtomicCounterBuffer->Bind(0);
    mTimer->Start();
#ifdef DEBUG_VOXEL
    GLuint* counterData = (GLuint*)mAtomicCounterBuffer->Map(BA_Write_Only);
    assert(counterData);
    counterData[0] = 0;
    counterData[1] = 0;
    mAtomicCounterBuffer->Unmap();
#else
    unsigned int counterData[2];
    counterData[0] = 0;
    counterData[1] = 0;
    mAtomicCounterBuffer->Clear(BIF_R32UI, BF_R32UI, BCT_Unsigned_Int, counterData);
#endif
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Reset Counter Time", workLoad);


    //--------------------------------- Begin building SVO ---------------------------------//

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
    for( ; curLevel < mSVOMaxLevel; ++curLevel )
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

    // Splat SVO leaf nodes pass.
    mTimer->Start();
    mVoxelFragmentListBuffer->Bind(1);
    mSVOBuffer->Bind(3);
    mBuildSVOTask->DispatchVertexIndirect(BUILD_SVO_SPLAT_LEAF_NODES_PASS,
        mVoxelFragmentListBuffer, 0);
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Build SVO Splat Leaf Nodes Pass", workLoad);
#ifdef DEBUG_VOXEL
    mSVOBuffer->Bind();
    svoBufferData = mSVOBuffer->Map(BA_Read_Only);
    svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
    svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
    mSVOBuffer->Unmap();
#endif

    //--------------------------------- End building SVO ---------------------------------//

    glViewport(0, 0, Width, Height);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Visualize SVO pass.
    if( mShowSVOBoxBV )
    {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        mSVOBuffer->BindToIndirect();
        mSVONodeCubeModel->Render(0, 0);
    }

    // Visualize scene voxelization pass.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mTimer->Start();
    ShowVoxelization();
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetTimingLabelValue("Visualization Pass", workLoad);

    // Show ray segment.
    if( mSVORaySegment )
    {
        mVoxelFragmentListBuffer->Bind(1);
        mSVOBuffer->Bind(3);
        mTimer->Start();
        mSVORayIntersectionTask->DispatchCompute(0, 1, 1, 1);
        mTimer->Stop();
        workLoad = mTimer->GetTimeElapsed();
        infoPanel->SetTimingLabelValue("SVO Ray Intersection Pass", workLoad);
#ifdef DEBUG_VOXEL
        mSVOBuffer->Bind();
        svoBufferData = mSVOBuffer->Map(BA_Read_Only);
        svoBufferHeadData = (SVONodeBufferHead*)svoBufferData;
        svoNodeTileData = (SVONodeTile*)(svoBufferHeadData + 1);
        mSVOBuffer->Unmap();
#endif

        mSVORaySegment->Render(0, 0);
    }
}
//----------------------------------------------------------------------------
void SVOApp::Terminate()
{
	// Release all resources.

    mAtomicCounterBuffer = 0;
    mVoxelFragmentListBuffer = 0;
    mSVOBuffer = 0;
    mSVOUniformBuffer = 0;

    mGatherVoxelFragmentListInfoTask = 0;
    mBuildSVOTask = 0;
    mSVORayIntersectionTask = 0;

	mGround = 0;
	mCeiling = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;
    mSVONodeCubeModel = 0;
    mSVORaySegment = 0;

    mTimer = 0;
}
//----------------------------------------------------------------------------
void SVOApp::ProcessInput()
{
	if (glfwGetKey(Window, GLFW_KEY_1) == GLFW_PRESS)
	{
        mShowMode = SM_WorldPosition;
	}
	if (glfwGetKey(Window, GLFW_KEY_2) == GLFW_PRESS)
	{
		mShowMode = SM_Scene;
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

    ShaderProgramInfo svoRaySegmentProgramInfo;
    svoRaySegmentProgramInfo.VShaderFileName = "SparseVoxelOctree/vSVORaySegment.glsl";
    svoRaySegmentProgramInfo.FShaderFileName = "SparseVoxelOctree/fSVORaySegment.glsl";
    svoRaySegmentProgramInfo.ShaderStageFlag = ShaderType::ST_Vertex |
                                               ShaderType::ST_Fragment;
    Pass* passSVORaySegment = new Pass(svoRaySegmentProgramInfo);

    Technique* techSVORaySegment = new Technique();
    techSVORaySegment->AddPass(passSVORaySegment);
    MaterialTemplate* mtSVORaySegment = new MaterialTemplate();
    mtSVORaySegment->AddTechnique(techSVORaySegment);
    Material* material = new Material(mtSVORaySegment);
    mSVORaySegment = new SVORaySegment(material, mMainCamera);
    mSVORaySegment->LineWidth = 1.0f;
    std::vector<int> temp;
    temp.reserve(1);
    temp.push_back(2);
    mSVORaySegment->LoadFromMemory(1, temp, 2, RaySegment);
    mSVORaySegment->CreateDeviceResource(mDevice);
}
//----------------------------------------------------------------------------
void SVOApp::OnCheckBoxClick(System::Object^ sender, System::EventArgs^ e)
{
    CheckBox^ checkBox = (CheckBox^)sender;

    if( checkBox->Name == "Show SVO Node BV" )
    {
        mShowSVOBoxBV = checkBox->Checked;
    }
}
//----------------------------------------------------------------------------