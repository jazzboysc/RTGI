#include "GPGPUMatrixVectorMathApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GPGPUMatrixVectorMathApp::GPGPUMatrixVectorMathApp(int, int)
	:
	mWindowTitle("GPGPU_MatrixVectorMath demo")
{
}
//----------------------------------------------------------------------------
GPGPUMatrixVectorMathApp::~GPGPUMatrixVectorMathApp()
{
}
//----------------------------------------------------------------------------
void GPGPUMatrixVectorMathApp::Initialize()
{
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());
}
//----------------------------------------------------------------------------
void GPGPUMatrixVectorMathApp::Run()
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
void GPGPUMatrixVectorMathApp::Terminate()
{
}
//----------------------------------------------------------------------------
void GPGPUMatrixVectorMathApp::InitializeOpenCL()
{
    ComputingApplication::InitializeOpenCL();

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
    
    mMatrix = new MemoryObject();
    mMatrix->CreateFromSystemMemory(mOpenCLContext,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*16, mat);
    mVector = new MemoryObject();
    mVector->CreateFromSystemMemory(mOpenCLContext,
        CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float)*4, vec);
    mResult = new MemoryObject();
    mResult->CreateFromSystemMemory(mOpenCLContext, CL_MEM_WRITE_ONLY,
        sizeof(float)*4, NULL);
	mKernel->SetArgument(0, mMatrix);
	mKernel->SetArgument(1, mVector);
	mKernel->SetArgument(2, mResult);
}
//----------------------------------------------------------------------------
void GPGPUMatrixVectorMathApp::TerminateOpenCL()
{
    mProgram = 0;
    mKernel = 0;
    mCommandQueue = 0;
    mMatrix = 0;
    mVector = 0;
    mResult = 0;
    
    ComputingApplication::TerminateOpenCL();
}
//----------------------------------------------------------------------------
void GPGPUMatrixVectorMathApp::OnKeyboard(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPUMatrixVectorMathApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPUMatrixVectorMathApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPUMatrixVectorMathApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPUMatrixVectorMathApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------