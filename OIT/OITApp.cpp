#include "OITApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
OITApp::OITApp(int width, int height)
	:
    mWidth(width),
    mHeight(height),
	mWindowTitle("OIT demo")
{
}
//----------------------------------------------------------------------------
OITApp::~OITApp()
{
}
//----------------------------------------------------------------------------
void OITApp::Initialize()
{
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.5f;
	glClearColor(color, color, color, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create camera.
	mCamera = new Camera;
	mCamera->SetFrustum(45.0f, (float)mWidth/(float)mHeight, 0.01f, 100.0f);
	mCamera->SetLookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
	Pass* passRenderBuffer = new Pass("vGPUABuffer.glsl", "fGPUABuffer.glsl");
	Technique* techRenderBuffer = new Technique();
	techRenderBuffer->AddPass(passRenderBuffer);
	MaterialTemplate* mtRenderBuffer = new MaterialTemplate();
	mtRenderBuffer->AddTechnique(techRenderBuffer);
    
	// Create scene.
	material = new Material(mtRenderBuffer);
	mModel = new OITTriMesh(material, mCamera);
	mModel->LoadFromFile("beethoven.ply");
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(0.0f, 0.0f, -10.0f));

	// Create head pointer texture.
	mHeadPointerTexture = new Texture2D();
	mHeadPointerTexture->CreateRenderTarget(mWidth, mHeight, 
		Texture2D::RTF_R32UI);

	// Create head pointer texture init data.
	int pixelCount = mWidth * mHeight;
	mHeadPointerTextureInitData = new PixelBuffer();
	mHeadPointerTextureInitData->ReserveDeviceResource(
		pixelCount*sizeof(GLuint), GL_STATIC_DRAW);
	void* pixelBufferData = mHeadPointerTextureInitData->Map(GL_WRITE_ONLY);
	memset(pixelBufferData, 0xFF, pixelCount*sizeof(GLuint));
	mHeadPointerTextureInitData->Unmap();

	// Create GPU memory allocator counter.
	mGPUMemAllocCounter = new AtomicCounterBuffer();
	mGPUMemAllocCounter->ReserveDeviceResource(sizeof(GLuint),
		GL_DYNAMIC_COPY);

	// Create GPU memory pool for concurrent linked lists.
	size_t gpuMemPoolSize = 2 * pixelCount * sizeof(vec4);
	mGPUMemPool = new TextureBuffer();
	mGPUMemPool->ReserveDeviceResource(gpuMemPoolSize, GL_DYNAMIC_COPY);
}
//----------------------------------------------------------------------------
void OITApp::Run()
{
    static float angle = 0.0f;
    angle += 0.5f;
    mat4 rot = RotateY(angle);
    vec3 trans = mModel->GetWorldTranslation();
    mModel->SetWorldTransform(rot);
    mModel->SetWorldTranslation(trans);

	// Reset head pointer texture and bind it to image unit 0.
	mHeadPointerTexture->UpdateFromPixelBuffer(mHeadPointerTextureInitData);
	mHeadPointerTexture->BindToImageUnit(0, GL_READ_WRITE);

	// Reset atomic counter.
	GLuint zero = 0;
	mGPUMemAllocCounter->UpdateSubData(0, 0, sizeof(zero), &zero);
 
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mModel->Render(0, 0);
    
	glutSwapBuffers();
}
//----------------------------------------------------------------------------
void OITApp::Terminate()
{
	mCamera = 0;
	mHeadPointerTexture = 0;
	mHeadPointerTextureInitData = 0;
	mGPUMemAllocCounter = 0;
	mGPUMemPool = 0;
    mModel = 0;
}
//----------------------------------------------------------------------------
void OITApp::OnKeyboard(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void OITApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void OITApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void OITApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void OITApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------