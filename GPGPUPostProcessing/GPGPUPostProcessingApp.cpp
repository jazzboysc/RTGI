#include "GPGPUPostProcessingApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GPGPUPostProcessingApp::GPGPUPostProcessingApp(int, int)
	:
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
    InitializeOpenCL();
    
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());

	float color = 0.5f;
	glClearColor(color, color, color, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Create camera.
	mCamera = new Camera;
	mCamera->SetFrustum(45.0f, (float)mWidth/(float)mHeight, 1.0f, 100.0f);
	mCamera->SetLookAt(vec3(0.0f, 8.0f, 25.0f), vec3(0.0f, 0.0f, 0.0f), 
		vec3(0.0f, 1.0f, 0.0f));

	// Create material templates.
	Material* material = 0;
	Pass* passRenderBuffer = new Pass("vRenderBuffer.glsl", "fRenderBuffer.glsl");
	Technique* techRenderBuffer = new Technique();
	techRenderBuffer->AddPass(passRenderBuffer);
	MaterialTemplate* mtRenderBuffer = new MaterialTemplate();
	mtRenderBuffer->AddTechnique(techRenderBuffer);
    
    mProgram = new ComputeProgram(mOpenCLContext, mOpenCLDevice, "matvec.clsl");
    mProgram->CreateDeviceResource();
    mKernel = new ComputeKernel(mProgram, "matvec_mult");
    
    mCommandQueue = new CommandQueue(mOpenCLContext, mOpenCLDevice);
    
    float mat[16], vec[4], cpuResult[4];
    for( int i = 0; i < 16; ++i )
    {
        mat[i] = i * 2.0f;
    }
    for( int i = 0; i < 4; ++i )
    {
        vec[i] = i * 3.0f;
        cpuResult[0] += mat[i     ] * vec[i];
        cpuResult[1] += mat[i +  4] * vec[i];
        cpuResult[2] += mat[i +  8] * vec[i];
        cpuResult[3] += mat[i + 12] * vec[i];
    }
 
    mMatrix = new MemoryObject(mOpenCLContext,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*16, mat);
    mVector = new MemoryObject(mOpenCLContext,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*4, vec);
    mResult = new MemoryObject(mOpenCLContext, CL_MEM_WRITE_ONLY,
        sizeof(float)*4, NULL);
	mKernel->SetArgument(0, mMatrix);
	mKernel->SetArgument(1, mVector);
	mKernel->SetArgument(2, mResult);
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::Run()
{
	static bool done = false;
	static float gpuResult[4];
	if( !done )
	{
		size_t workUnitsPerKernel = 4;
		mCommandQueue->EnqueueKernel(mKernel, 1, &workUnitsPerKernel, NULL);
		mCommandQueue->EnqueueReadBuffer(mResult, 0, sizeof(float)*4, gpuResult);
		done = true;
	}
}
//----------------------------------------------------------------------------
void GPGPUPostProcessingApp::Terminate()
{
    mProgram = 0;
    mKernel = 0;
    mCommandQueue = 0;
    mMatrix = 0;
    mVector = 0;
    mResult = 0;

	mCamera = 0;
	mRenderBuffer = 0;
	mRenderTexture = 0;
    
    TerminateOpenCL();
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