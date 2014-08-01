#include "RayBundleApp.h"
#include "RNG.h"

using namespace RTGI;

//----------------------------------------------------------------------------
RayBundleApp::RayBundleApp(int width, int height)
	:
	mWidth(width),
	mHeight(height),
	mWindowTitle("Ray-bundle demo")
{
}
//----------------------------------------------------------------------------
RayBundleApp::~RayBundleApp()
{
}
//----------------------------------------------------------------------------
void RayBundleApp::Initialize()
{
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.5f;
	glClearColor(color, color, color, 0.0f);
    //glDisable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glDisable(GL_CULL_FACE);

	// Create ray-bundle projector.
	mRayBundleProjector = new Camera;
	mRayBundleProjector->SetFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 50.0f);
	mRayBundleProjector->SetLookAt(vec3(10.0f, 23.0f, 32.0f), vec3(-7.0f, 0.0f, -12.0f),
		vec3(0.0f, 1.0f, 0.0f));
    
    // Create scene camera.
	mCamera = new Camera;
	mCamera->SetFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 50.0f);
	mCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
	Pass* passScene = new Pass("vScene.glsl", "fScene.glsl");
	Technique* techScene = new Technique();
	techScene->AddPass(passScene);
	MaterialTemplate* mtScene = new MaterialTemplate();
	mtScene->AddTechnique(techScene);

	// Create scene.
	mat4 rotM;
	material = new Material(mtScene);
	mModel = new RayBundleTriMesh(material, mCamera);
	mModel->LoadFromFile("beethoven.ply");
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(-2.0f, 5.8f, -1.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);

	material = new Material(mtScene);
	mGround = new RayBundleTriMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(0.75f, 0.75f, 0.75f);

	material = new Material(mtScene);
	mCeiling = new RayBundleTriMesh(material, mCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.75f, 0.75f, 0.75f);

	material = new Material(mtScene);
	mLight = new RayBundleTriMesh(material, mCamera);
	mLight->LoadFromFile("square.ply");
	mLight->GenerateNormals();
	mLight->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mLight->SetWorldTransform(rotM);
	mLight->SetWorldScale(vec3(0.5f));
	mLight->SetWorldTranslation(vec3(0.0f, 19.0f, 3.0f));
    mLight->MaterialColor = vec3(1.0f, 1.0f, 1.0f);

	material = new Material(mtScene);
	mBackWall = new RayBundleTriMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);

	material = new Material(mtScene);
	mLeftWall = new RayBundleTriMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);

	material = new Material(mtScene);
	mRightWall = new RayBundleTriMesh(material, mCamera);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource();
	rotM = RotateZ(90.0f);
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);
}
//----------------------------------------------------------------------------
void RayBundleApp::DrawScene()
{
	mGround->Render(0, 0);
	mCeiling->Render(0, 0);
	mLight->Render(0, 0);
	mBackWall->Render(0, 0);
	mLeftWall->Render(0, 0);
	mRightWall->Render(0, 0);
	mModel->Render(0, 0);
}
//----------------------------------------------------------------------------
void RayBundleApp::Run()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawScene();

	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void RayBundleApp::Terminate()
{
	// Release all resources.

    delete mRayBundleProjector;
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
void RayBundleApp::OnKeyboard(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void RayBundleApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void RayBundleApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void RayBundleApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void RayBundleApp::OnReshape(int x, int y)
{
	mWidth = x;
	mHeight = y;
}
//----------------------------------------------------------------------------