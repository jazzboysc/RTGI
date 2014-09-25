#include "GPGPUPostProcessingApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GPGPUPostProcessingApp::GPGPUPostProcessingApp(int width, int height)
	:
    mWidth(width),
    mHeight(height),
	mWindowTitle("GPGPU post processing demo")
{
}
//----------------------------------------------------------------------------
GPGPUPostProcessingApp::~GPGPUPostProcessingApp()
{
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::Initialize()
{
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.5f;
	glClearColor(color, color, color, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create camera.
	mCamera = new Camera;
	mCamera->SetPerspectiveFrustum(45.0f, (float)mWidth/(float)mHeight, 0.01f, 100.0f);
	mCamera->SetLookAt(vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 0.0f),
		vec3(0.0f, 1.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
	Pass* passRenderBuffer = new Pass("vRenderBuffer.glsl", "fRenderBuffer.glsl");
	Technique* techRenderBuffer = new Technique();
	techRenderBuffer->AddPass(passRenderBuffer);
	MaterialTemplate* mtRenderBuffer = new MaterialTemplate();
	mtRenderBuffer->AddTechnique(techRenderBuffer);
    
	Pass* passScreenQuad = new Pass("vGPGPUPostProcessingScreenQuad.glsl",
        "fGPGPUPostProcessingScreenQuad.glsl");
	Technique* techScreenQuad = new Technique();
	techScreenQuad->AddPass(passScreenQuad);
	MaterialTemplate* mtScreenQuad = new MaterialTemplate();
	mtScreenQuad->AddTechnique(techScreenQuad);
    
	// Create render target.
	mRenderTexture = new Texture2D();
	mRenderTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
	mDepthTexture = new Texture2D();
	mDepthTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_Depth);
    mResultTexture = new Texture2D();
    mResultTexture->CreateRenderTarget(mWidth, mHeight, Texture2D::TF_RGBF);
    
	// Create framebuffer.
	Texture2D* renderTexture[1] = {mRenderTexture};
	mRenderBuffer = new FrameBuffer();
	mRenderBuffer->SetRenderTargets(1, renderTexture, mDepthTexture);
    
	// Create scene.
	material = new Material(mtRenderBuffer);
	mModel = new GPGPUPostProcessingTriMesh(material, mCamera);
	mModel->LoadFromFile("beethoven.ply");
	mModel->GenerateNormals();
	mModel->CreateDeviceResource();
	mModel->SetWorldTranslation(vec3(0.0f, 0.0f, -10.0f));
    
	// Create screen quad.
	material = new Material(mtScreenQuad);
	mScreenQuad = new GPGPUPostProcessingScreenQuad(material);
	mScreenQuad->LoadFromFile("screenquad.ply");
	mScreenQuad->SetTCoord(0, vec2(0.0f, 0.0f));
	mScreenQuad->SetTCoord(1, vec2(1.0f, 0.0f));
	mScreenQuad->SetTCoord(2, vec2(1.0f, 1.0f));
	mScreenQuad->SetTCoord(3, vec2(0.0f, 1.0f));
	mScreenQuad->CreateDeviceResource();
	mScreenQuad->RenderTexture = mResultTexture;
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::Run()
{
    static float angle = 0.0f;
    angle += 0.5f;
    mat4 rot = RotateY(angle);
    vec3 trans = mModel->GetWorldTranslation();
    mModel->SetWorldTransform(rot);
    mModel->SetWorldTranslation(trans);
 
    // OpenGL stuff.
    
    //mRenderBuffer->Enable();
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    mModel->Render(0, 0);
    //mRenderBuffer->Disable();
    
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //mScreenQuad->Render(0, 0);
    
	glutSwapBuffers();
    
    // OpenCL stuff.
    
    //size_t globalWorkSize[] = { mWidth, mHeight };
    //mCommandQueue->ExecuteKernel(mKernel, 2, globalWorkSize, NULL);
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::Terminate()
{
	mCamera = 0;
	mRenderBuffer = 0;
	mRenderTexture = 0;
    mDepthTexture = 0;
    mResultTexture = 0;
    mModel = 0;
    mScreenQuad = 0;
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::InitializeOpenCL()
{
    //Application::InitializeOpenCL();
    //
    //// Create compute program.
    //mProgram = new ComputeProgram(mOpenCLContext, mOpenCLDevice, "filter.clsl");
    //mProgram->CreateDeviceResource();
    //mKernel = new ComputeKernel(mProgram, "filter");
    //
    //// Create command queue.
    //mCommandQueue = new CommandQueue(mOpenCLContext, mOpenCLDevice);
    //
    //// Create kernel image object.
    //mKernelRenderTexture = new MemoryObject();
    //mKernelRenderTexture->CreateFromTexture2D(mOpenCLContext,
    //    CL_MEM_READ_ONLY, mRenderTexture);
    //mKernelResultTexture = new MemoryObject();
    //mKernelResultTexture->CreateFromTexture2D(mOpenCLContext,
    //    CL_MEM_WRITE_ONLY, mResultTexture);
    //
    //mKernel->SetArgument(0, mKernelResultTexture);
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::TerminateOpenCL()
{
    mProgram = 0;
    mKernel = 0;
    mCommandQueue = 0;
    mKernelRenderTexture = 0;
    mKernelResultTexture = 0;
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::OnKeyboard(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------