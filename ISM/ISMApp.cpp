#include "ISMApp.h"
#include "RNG.h"

using namespace RTGI;

//----------------------------------------------------------------------------
ISMApp::ISMApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("ISM demo")
{
}
//----------------------------------------------------------------------------
ISMApp::~ISMApp()
{
}
//----------------------------------------------------------------------------
void ISMApp::Initialize()
{
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    // Create scene camera.
	mCamera = new Camera();
	mCamera->SetPerspectiveFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 50.0f);
	mCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
    ShaderProgramInfo programInfo;
    programInfo.VShaderFileName = "vScene.glsl";
    programInfo.FShaderFileName = "fScene.glsl";
    programInfo.TCShaderFileName = "tcScene.glsl";
    programInfo.TEShaderFileName = "teScene.glsl";
    programInfo.ShaderStageFlag = ShaderType::ST_Vertex | 
                                  ShaderType::ST_Fragment | 
                                  ShaderType::ST_TessellationControl |
                                  ShaderType::ST_TessellationEvaluation;
    Pass* passScene = new Pass(programInfo);
	Technique* techScene = new Technique();
	techScene->AddPass(passScene);
	MaterialTemplate* mtScene = new MaterialTemplate();
	mtScene->AddTechnique(techScene);

	// Create scene.
	mat4 rotM;
	material = new Material(mtScene);
	mModel = new ISMTriMesh(material, mCamera);
	mModel->LoadFromFile("beethoven.ply");
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(-2.0f, 5.8f, -1.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);

	material = new Material(mtScene);
	mGround = new ISMTriMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(0.5f, 0.0f, 0.0f);

	material = new Material(mtScene);
	mCeiling = new ISMTriMesh(material, mCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.0f, 0.0f, 0.75f);

	material = new Material(mtScene);
	mLight = new ISMTriMesh(material, mCamera);
	mLight->LoadFromFile("square.ply");
	mLight->GenerateNormals();
	mLight->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mLight->SetWorldTransform(rotM);
	mLight->SetWorldScale(vec3(0.5f));
	mLight->SetWorldTranslation(vec3(0.0f, 19.0f, 3.0f));
	mLight->MaterialColor = vec3(0.0f, 0.0f, 0.0f);
	mLight->IsLight = true;

	material = new Material(mtScene);
	mBackWall = new ISMTriMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);

	material = new Material(mtScene);
	mLeftWall = new ISMTriMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);

	material = new Material(mtScene);
	mRightWall = new ISMTriMesh(material, mCamera);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource();
	rotM = RotateZ(90.0f);
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);

}
//----------------------------------------------------------------------------
void ISMApp::DrawScene()
{
	mGround->SetCamera(mCamera);
	mGround->Render(0, 0);

	mCeiling->SetCamera(mCamera);
	mCeiling->Render(0, 0);

	mLight->SetCamera(mCamera);
	mLight->Render(0, 0);

	mBackWall->SetCamera(mCamera);
	mBackWall->Render(0, 0);

	mLeftWall->SetCamera(mCamera);
	mLeftWall->Render(0, 0);

	mRightWall->SetCamera(mCamera);
	mRightWall->Render(0, 0);

	mModel->SetCamera(mCamera);
	mModel->Render(0, 0);
}
//----------------------------------------------------------------------------
void ISMApp::Run()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	DrawScene();

	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void ISMApp::Terminate()
{
	// Release all resources.

	delete mCamera;

	mGround = 0;
	mCeiling = 0;
	mLight = 0;
	mBackWall = 0;
	mLeftWall = 0;
	mRightWall = 0;
	mModel = 0;
}
//----------------------------------------------------------------------------
void ISMApp::OnKeyboard(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void ISMApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void ISMApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void ISMApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void ISMApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------