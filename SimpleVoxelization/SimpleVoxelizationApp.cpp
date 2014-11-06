#include "SimpleVoxelizationApp.h"
#include "RNG.h"
#include "InformationPanel.h"

using namespace RTGI;
using namespace RTGI::GUIFramework;

//----------------------------------------------------------------------------
SimpleVoxelizationApp::SimpleVoxelizationApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("Simple voxelization demo")
{
    mIsRotatingModel = false;
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
    assert(globalX >= VOXEL_DIMENSION && globalY >= VOXEL_DIMENSION && globalZ >= VOXEL_DIMENSION);

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

    // Create scene voxel buffer.
    mVoxelBuffer = new StructuredBuffer();
    GLuint voxelCount = VOXEL_DIMENSION * VOXEL_DIMENSION * VOXEL_DIMENSION;
    GLuint bufferSize = voxelCount * sizeof(GLuint) * 4;
    mVoxelBuffer->ReserveDeviceResource(bufferSize, GL_DYNAMIC_COPY);
    memset(mZeroBuffer, 0x00, bufferSize);

    // Create indirect command buffer.
    mIndirectCommandBuffer = new StructuredBuffer();
    bufferSize = sizeof(GLuint) * 5;
    mIndirectCommandBuffer->ReserveDeviceResource(bufferSize, GL_DYNAMIC_COPY);

	// Create scene.
	mat4 rotM;
    material = new Material(mtVoxelization);
	mModel = new SimpleVoxelizationTriMesh(material, mVoxelizationProjector);
	mModel->LoadFromFile("dragon_s.ply");
    mat4 scale = Scale(vec3(60.0f));
    mModel->UpdateModelSpaceVertices(scale);
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
    mModel->SetWorldTranslation(vec3(0.0f, 4.0f, 3.0f));
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

    // Create labels.
    InformationPanel::GetInstance()->AddLabel("Reset Voxel Buffer Pass", 16, 20);
    InformationPanel::GetInstance()->AddLabel("Voxelization Pass", 16, 40);

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

    glViewport(0, 0, VOXEL_DIMENSION >> 3, VOXEL_DIMENSION >> 3);
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

    // Reset voxel buffer pass.
    mTimer->Start();
    mResetVoxelBufferTask->Dispatch(0, VOXEL_DIMENSION, VOXEL_DIMENSION, VOXEL_DIMENSION);
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
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    mTimer->Start();
	VoxelizeScene();
    mTimer->Stop();
    workLoad = mTimer->GetTimeElapsed();
    infoPanel->SetLabelValue("Voxelization Pass", workLoad);

    // Gather voxel buffer pass.
    mGatherVoxelBufferTask->Dispatch(0, VOXEL_DIMENSION, VOXEL_DIMENSION, VOXEL_DIMENSION);
    //mIndirectCommandBuffer->Map(GL_)

    // Visualize scene voxelization pass.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glViewport(0, 0, mWidth, mHeight);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ShowVoxelization();

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
    mVoxelBuffer = 0;
    mIndirectCommandBuffer = 0;

	mGround = 0;
	mCeiling = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;

    mTimer = 0;
}
//----------------------------------------------------------------------------
void SimpleVoxelizationApp::OnKeyboard(unsigned char key, int x, int y)
{
    switch( key )
    {
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