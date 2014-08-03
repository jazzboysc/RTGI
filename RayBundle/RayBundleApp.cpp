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

	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	float sphereRadius = 14.15f;
	vec3 sphereCenter = vec3(0.0f, 10.0f, 0.0f);
	// Create ray-bundle projector.
	mRayBundleProjector = new Camera(false);
	mRayBundleProjector->SetOrthogonalFrustum(11.0f, (float)mWidth/(float)mHeight, 1.0f, 100.0f);
	mRayBundleProjector->SetLookAt(vec3(0.0f, -4.15f, 0.0f), sphereCenter,
		vec3(0.0f, 0.0f, 1.0f));
    
    // Create scene camera.
	mCamera = new Camera();
	mCamera->SetPerspectiveFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 50.0f);
	//mCamera->SetOrthogonalFrustum(15.0f, (float)mWidth/(float)mHeight, 1.0f, 500.0f);
	mCamera->SetLookAt(vec3(0.0f, 10.0f, 35.0f), vec3(0.0f, 10.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
	Pass* passScene = new Pass("vScene.glsl", "fScene.glsl");
	Technique* techScene = new Technique();
	techScene->AddPass(passScene);
	MaterialTemplate* mtScene = new MaterialTemplate();
	mtScene->AddTechnique(techScene);

	Pass* passCreateAccumulation = new Pass("vCreateAccumulation.glsl", "fCreateAccumulation.glsl");
	Technique* techRayBundle = new Technique();
	techRayBundle->AddPass(passCreateAccumulation);
	MaterialTemplate* mtRayBundle = new MaterialTemplate();
	mtRayBundle->AddTechnique(techRayBundle);

	// Create scene.
	mat4 rotM;
	material = new Material(mtRayBundle);
	mModel = new RayBundleTriMesh(material, mCamera);
	mModel->LoadFromFile("beethoven.ply");
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(-2.0f, 5.8f, -1.0f));
	mModel->MaterialColor = vec3(0.65f, 0.65f, 0.65f);

	material = new Material(mtRayBundle);
	mGround = new RayBundleTriMesh(material, mCamera);
	mGround->LoadFromFile("square.ply");
	mGround->GenerateNormals();
	mGround->CreateDeviceResource();
    mGround->MaterialColor = vec3(0.0f, 0.75f, 0.75f);

	material = new Material(mtRayBundle);
	mCeiling = new RayBundleTriMesh(material, mCamera);
	mCeiling->LoadFromFile("square.ply");
	mCeiling->GenerateNormals();
	mCeiling->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mCeiling->SetWorldTransform(rotM);
	mCeiling->SetWorldTranslation(vec3(0.0f, 20.0f, 0.0f));
    mCeiling->MaterialColor = vec3(0.75f, 0.75f, 0.75f);

	material = new Material(mtRayBundle);
	mLight = new RayBundleTriMesh(material, mCamera);
	mLight->LoadFromFile("square.ply");
	mLight->GenerateNormals();
	mLight->CreateDeviceResource();
	rotM = RotateX(180.0f);
	mLight->SetWorldTransform(rotM);
	mLight->SetWorldScale(vec3(0.5f));
	mLight->SetWorldTranslation(vec3(0.0f, 19.0f, 3.0f));
    mLight->MaterialColor = vec3(1.0f, 1.0f, 1.0f);

	material = new Material(mtRayBundle);
	mBackWall = new RayBundleTriMesh(material, mCamera);
	mBackWall->LoadFromFile("square.ply");
	mBackWall->GenerateNormals();
	mBackWall->CreateDeviceResource();
	rotM = RotateX(90.0f);
	mBackWall->SetWorldTransform(rotM);
	mBackWall->SetWorldTranslation(vec3(0.0f, 10.0f, -10.0f));
    mBackWall->MaterialColor = vec3(0.75f, 0.75f, 0.75f);

	material = new Material(mtRayBundle);
	mLeftWall = new RayBundleTriMesh(material, mCamera);
	mLeftWall->LoadFromFile("square.ply");
	mLeftWall->GenerateNormals();
	mLeftWall->CreateDeviceResource();
	rotM = RotateZ(-90.0f);
	mLeftWall->SetWorldTransform(rotM);
	mLeftWall->SetWorldTranslation(vec3(-10.0f, 10.0f, 0.0f));
    mLeftWall->MaterialColor = vec3(1.0f, 0.0f, 0.0f);

	material = new Material(mtRayBundle);
	mRightWall = new RayBundleTriMesh(material, mCamera);
	mRightWall->LoadFromFile("square.ply");
	mRightWall->GenerateNormals();
	mRightWall->CreateDeviceResource();
	rotM = RotateZ(90.0f);
	mRightWall->SetWorldTransform(rotM);
	mRightWall->SetWorldTranslation(vec3(10.0f, 10.0f, 0.0f));
    mRightWall->MaterialColor = vec3(0.0f, 1.0f, 0.0f);

	// Create head pointer texture.
	mHeadPointerTexture = new Texture2D();
	mHeadPointerTexture->CreateRenderTarget(mWidth, mHeight, 
		Texture2D::RTF_R32UI);

	// Create head pointer texture init data.
	int pixelCount = mWidth * mHeight;
	mHeadPointerTextureInitData = new PixelBuffer();
	mHeadPointerTextureInitData->ReserveDeviceResource(
		pixelCount*sizeof(GLuint), GL_STATIC_DRAW);
	mHeadPointerTextureInitData->Bind();
	void* pixelBufferData = mHeadPointerTextureInitData->Map(GL_WRITE_ONLY);
	assert( pixelBufferData );
	memset(pixelBufferData, 0x00, pixelCount*sizeof(GLuint));
	mHeadPointerTextureInitData->Unmap();

	// Create GPU memory allocator counter.
	mGPUMemAllocCounter = new AtomicCounterBuffer();
	mGPUMemAllocCounter->ReserveDeviceResource(sizeof(GLuint),
		GL_DYNAMIC_COPY);

	// Create GPU memory pool for concurrent linked lists.
	size_t gpuMemPoolSize = 8 * pixelCount * sizeof(vec4);
	mGPUMemPool = new TextureBuffer();
	mGPUMemPool->ReserveDeviceResource(gpuMemPoolSize, GL_DYNAMIC_COPY);

	// Create GPU memory pool texture.
	mGPUMemPoolTexture = new Texture2D();
	mGPUMemPoolTexture->LoadFromTextureBuffer(mGPUMemPool, GL_RGBA32UI);
}
//----------------------------------------------------------------------------
void RayBundleApp::DrawScene()
{
	//mGround->SetCamera(mRayBundleProjector);
	//mGround->Render(0, 0);

	//mCeiling->SetCamera(mRayBundleProjector);
	//mCeiling->Render(0, 0);

	//mLight->SetCamera(mRayBundleProjector);
	//mLight->Render(0, 0);

	//mBackWall->SetCamera(mRayBundleProjector);
	//mBackWall->Render(0, 0);

	//mLeftWall->SetCamera(mRayBundleProjector);
	//mLeftWall->Render(0, 0);

	//mRightWall->SetCamera(mRayBundleProjector);
	//mRightWall->Render(0, 0);

	//mModel->SetCamera(mRayBundleProjector);
	//mModel->Render(0, 0);

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
void RayBundleApp::Run()
{
	// Reset atomic counter.
	mGPUMemAllocCounter->Bind(0);
	GLuint* counterData = (GLuint*)mGPUMemAllocCounter->Map(GL_WRITE_ONLY);
	assert( counterData );
	counterData[0] = 0;
	mGPUMemAllocCounter->Unmap();

	// Reset head pointer texture.
	mHeadPointerTexture->UpdateFromPixelBuffer(mHeadPointerTextureInitData);

	// Bind textures to image units.
	mHeadPointerTexture->BindToImageUnit(0, GL_READ_WRITE);
	mGPUMemPoolTexture->BindToImageUnit(1, GL_READ_WRITE);

    glClear(GL_COLOR_BUFFER_BIT);

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

	mGPUMemAllocCounter = 0;
	mHeadPointerTexture = 0;
	mHeadPointerTextureInitData = 0;
	mGPUMemPoolTexture = 0;
	mGPUMemPool = 0;
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