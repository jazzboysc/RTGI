#include "GPGPU_MatrixVectorMathApp.h"

using namespace RTGI;

//----------------------------------------------------------------------------
GPGPU_MatrixVectorMathApp::GPGPU_MatrixVectorMathApp(int, int)
	:
	mWindowTitle("GPGPU_MatrixVectorMath demo")
{
}
//----------------------------------------------------------------------------
GPGPU_MatrixVectorMathApp::~GPGPU_MatrixVectorMathApp()
{
}
//----------------------------------------------------------------------------
void GPGPU_MatrixVectorMathApp::Initialize()
{
    InitializeOpenCL();
    
	std::string title = mWindowTitle;
	glutSetWindowTitle(title.c_str());
    
    mProgram = new ComputeProgram(mOpenCLContext, mOpenCLDevice, "matvec.clsl");
    mProgram->CreateDeviceResource();
    mKernel = new ComputeKernel(mProgram, "matvec_mult");
    
    mCommandQueue = new CommandQueue(mOpenCLContext, mOpenCLDevice);
    
    float mat[16], vec[4], gpuResult[4], cpuResult[4];
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
}
//----------------------------------------------------------------------------
void GPGPU_MatrixVectorMathApp::Run()
{
}
//----------------------------------------------------------------------------
void GPGPU_MatrixVectorMathApp::Terminate()
{
    mProgram = 0;
    mKernel = 0;
    mCommandQueue = 0;
    mMatrix = 0;
    mVector = 0;
    mResult = 0;
    
    TerminateOpenCL();
}
//----------------------------------------------------------------------------
void GPGPU_MatrixVectorMathApp::OnKeyboard(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPU_MatrixVectorMathApp::OnKeyboardUp(unsigned char key, int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPU_MatrixVectorMathApp::OnMouse(int button, int state, int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPU_MatrixVectorMathApp::OnMouseMove(int x, int y)
{
}
//----------------------------------------------------------------------------
void GPGPU_MatrixVectorMathApp::OnReshape(int x, int y)
{
}
//----------------------------------------------------------------------------