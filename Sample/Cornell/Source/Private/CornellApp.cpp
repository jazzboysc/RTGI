#include "CornellApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
CornellApp::CornellApp(int width, int height)
{
	Width = width;
	Height = height;
	Title = "Cornell demo";
}
//----------------------------------------------------------------------------
CornellApp::~CornellApp()
{
}
//----------------------------------------------------------------------------
void CornellApp::Initialize(GPUDevice* device)
{
	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create scene camera.
	mMainCamera->SetPerspectiveFrustum(45.0f, (float)Width/(float)Height, 1.0f, 50.0f);
	mMainCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo programInfo;
    programInfo.VShaderFileName = "Cornell/vScene.glsl";
    programInfo.FShaderFileName = "Cornell/fScene.glsl";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex | ShaderType::ST_Fragment;
    Pass* passScene = new Pass(programInfo);
	Technique* techScene = new Technique();
	techScene->AddPass(passScene);
	MaterialTemplate* mtScene = new MaterialTemplate();
	mtScene->AddTechnique(techScene);

	// Create scene.
	mat4 rotM;
	material = new Material(mtScene);
	mModel = new CornellTriMesh(material, mMainCamera);
	mModel->LoadFromPLYFile("beethoven.ply");
	mModel->GenerateNormals();
	mModel->CreateDeviceResource(mDevice);
	mModel->SetWorldTranslation(vec3(-2.0f, 5.8f, -1.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);

	material = new Material(mtScene);
	mGround = new CornellTriMesh(material, mMainCamera);
	mGround->LoadFromPLYFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource(mDevice);
    mGround->MaterialColor = vec3(0.5f, 0.0f, 0.0f);

	material = new Material(mtScene);
	mCeiling = new CornellTriMesh(material, mMainCamera);
	mCeiling->LoadFromPLYFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource(mDevice);
	rotM = rotate(mat4(), radians(180.0f), vec3(1, 0, 0));
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);

	material = new Material(mtScene);
	mLight = new CornellTriMesh(material, mMainCamera);
	mLight->LoadFromPLYFile("square.ply");
	mLight->GenerateNormals();
	mLight->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(180.0f), vec3(1, 0, 0));
	mLight->SetWorldTransform(rotM);
	mLight->SetWorldScale(vec3(0.5f));
	mLight->SetWorldTranslation(vec3(0.0f, 19.0f, 3.0f));
	mLight->MaterialColor = vec3(0.0f, 0.0f, 0.0f);
	mLight->IsLight = true;

	material = new Material(mtScene);
	mBackWall = new CornellTriMesh(material, mMainCamera);
	mBackWall->LoadFromPLYFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(1, 0, 0));
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);

	material = new Material(mtScene);
	mLeftWall = new CornellTriMesh(material, mMainCamera);
	mLeftWall->LoadFromPLYFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(-90.0f), vec3(0, 0, 1));
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);

	material = new Material(mtScene);
	mRightWall = new CornellTriMesh(material, mMainCamera);
	mRightWall->LoadFromPLYFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource(mDevice);
    rotM = rotate(mat4(), radians(90.0f), vec3(0, 0, 1));
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);

}
//----------------------------------------------------------------------------
void CornellApp::DrawScene()
{
	mGround->SetCamera(mMainCamera);
	mGround->Render(0, 0);

	mCeiling->SetCamera(mMainCamera);
	mCeiling->Render(0, 0);

	mLight->SetCamera(mMainCamera);
	mLight->Render(0, 0);

	mBackWall->SetCamera(mMainCamera);
	mBackWall->Render(0, 0);

	mLeftWall->SetCamera(mMainCamera);
	mLeftWall->Render(0, 0);

	mRightWall->SetCamera(mMainCamera);
	mRightWall->Render(0, 0);

	mModel->SetCamera(mMainCamera);
	mModel->Render(0, 0);
}
//----------------------------------------------------------------------------
void CornellApp::FrameFunc()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawScene();
}
//----------------------------------------------------------------------------
void CornellApp::Terminate()
{
	// Release all resources.

	mGround = 0;
	mCeiling = 0;
	mLight = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;
}
//----------------------------------------------------------------------------
void CornellApp::ProcessInput()
{
}
//----------------------------------------------------------------------------