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
	Pass* passRayBundle = new Pass("vRayBundle.glsl", "fRayBundle.glsl");
	Technique* techRayBundle = new Technique();
	techRayBundle->AddPass(passScene);
	techRayBundle->AddPass(passRayBundle);
	MaterialTemplate* mtRayBundle = new MaterialTemplate();
	mtRayBundle->AddTechnique(techRayBundle);

	Pass* passUpdateAccumulation = new Pass("vUpdateAccumulation.glsl", "fUpdateAccumulation.glsl");
	Technique* techUpdateAccumulation = new Technique();
	techUpdateAccumulation->AddPass(passUpdateAccumulation);
	MaterialTemplate* mtUpdateAccumulation = new MaterialTemplate();
	mtUpdateAccumulation->AddTechnique(techUpdateAccumulation);

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
	mLight->EmissionColor = vec3(1.0f, 1.0f, 1.0f);
	mLight->IsLight = true;

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

	int pixelCount = 0;
	void* pixelBufferData = 0;
	size_t gpuMemPoolSize = 0;

	// Create accumulation texture.
	mAccumulationTexture = new Texture3D();
	mAccumulationTexture->LoadFromSystemMemory(GL_RGBA32F_ARB, mWidth, mWidth, 
		mWidth, GL_RGBA, GL_FLOAT, 0);

	// Create accumulation texture init data.
	pixelCount = mWidth * mWidth * mWidth;
	mAccumulationTextureInitData = new PixelBuffer();
	mAccumulationTextureInitData->ReserveDeviceResource(
		pixelCount*sizeof(vec4), GL_STATIC_DRAW);
	mAccumulationTextureInitData->Bind();
	pixelBufferData = mAccumulationTextureInitData->Map(GL_WRITE_ONLY);
	assert( pixelBufferData );
	memset(pixelBufferData, 0x00, pixelCount*sizeof(vec4));
	mAccumulationTextureInitData->Unmap();

	// Create ray head pointer texture.
	mRayHeadPointerTexture = new Texture2D();
	mRayHeadPointerTexture->CreateRenderTarget(mWidth, mHeight, 
		Texture2D::RTF_R32UI);

	// Create ray head pointer texture init data.
	pixelCount = mWidth * mHeight;
	mRayHeadPointerTextureInitData = new PixelBuffer();
	mRayHeadPointerTextureInitData->ReserveDeviceResource(
		pixelCount*sizeof(GLuint), GL_STATIC_DRAW);
	mRayHeadPointerTextureInitData->Bind();
	pixelBufferData = mRayHeadPointerTextureInitData->Map(GL_WRITE_ONLY);
	assert( pixelBufferData );
	memset(pixelBufferData, 0x00, pixelCount*sizeof(GLuint));
	mRayHeadPointerTextureInitData->Unmap();

	// Create ray GPU memory allocator counter.
	mRayAllocCounter = new AtomicCounterBuffer();
	mRayAllocCounter->ReserveDeviceResource(sizeof(GLuint),
		GL_DYNAMIC_COPY);

	// Create ray GPU memory pool for concurrent linked lists.
	gpuMemPoolSize = 8 * pixelCount * (4*sizeof(vec4) + sizeof(GLuint) + 
		sizeof(GLfloat) + sizeof(GLboolean));
	mRayGPUMemPool = new StructuredBuffer();
	mRayGPUMemPool->ReserveDeviceResource(gpuMemPoolSize, GL_DYNAMIC_COPY);

	// Create update accumulation screen quad.
	material = new Material(mtUpdateAccumulation);
	mUpdateAccuScreenQuad = new UpdateAccumulationScreenQuad(material);
	mUpdateAccuScreenQuad->LoadFromFile("screenquad.ply");
	mUpdateAccuScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
	mUpdateAccuScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
	mUpdateAccuScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
	mUpdateAccuScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
	mUpdateAccuScreenQuad->CreateDeviceResource();
}
//----------------------------------------------------------------------------
void RayBundleApp::DrawRayBundle()
{
	mGround->SetCamera(mRayBundleProjector);
	mGround->Render(0, 1);

	mCeiling->SetCamera(mRayBundleProjector);
	mCeiling->Render(0, 1);

	mLight->SetCamera(mRayBundleProjector);
	mLight->Render(0, 1);

	mBackWall->SetCamera(mRayBundleProjector);
	mBackWall->Render(0, 1);

	mLeftWall->SetCamera(mRayBundleProjector);
	mLeftWall->Render(0, 1);

	mRightWall->SetCamera(mRayBundleProjector);
	mRightWall->Render(0, 1);

	mModel->SetCamera(mRayBundleProjector);
	mModel->Render(0, 1);
}
//----------------------------------------------------------------------------
void RayBundleApp::Run()
{
	// Reset accumulation texture.
	mAccumulationTexture->UpdateFromPixelBuffer(mAccumulationTextureInitData);

	// Reset ray bundle atomic counter.
	mRayAllocCounter->Bind(0);
	GLuint* counterData = (GLuint*)mRayAllocCounter->Map(GL_WRITE_ONLY);
	assert( counterData );
	counterData[0] = 0;
	mRayAllocCounter->Unmap();

	// Reset ray bundle head pointer texture.
	mRayHeadPointerTexture->UpdateFromPixelBuffer(
		mRayHeadPointerTextureInitData);

	// Bind ray bundle textures to image units.
	mRayHeadPointerTexture->BindToImageUnit(0, GL_READ_WRITE);
	mAccumulationTexture->BindToImageUnit(1, GL_READ_WRITE);

	// Bind ray bundle buffer.
	mRayGPUMemPool->Bind(0);

    glClear(GL_COLOR_BUFFER_BIT);

	// Create a ray bundle.
	DrawRayBundle();

	// Transfer energy for ray bundle.
	mUpdateAccuScreenQuad->Render(0, 0);

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

	mAccumulationTexture = 0;
	mAccumulationTextureInitData = 0;
	mRayAllocCounter = 0;
	mRayHeadPointerTexture = 0;
	mRayHeadPointerTextureInitData = 0;
	mRayGPUMemPool = 0;

	mUpdateAccuScreenQuad = 0;
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