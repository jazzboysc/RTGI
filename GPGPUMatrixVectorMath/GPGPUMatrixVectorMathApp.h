#ifndef RTGI_GPGPUMatrixVectorMathApp_H
#define RTGI_GPGPUMatrixVectorMathApp_H

#include "GraphicsFrameworkHeader.h"
#include "ComputingFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/11/2014
//----------------------------------------------------------------------------
class GPGPUMatrixVectorMathApp : public ComputingApplication
{
public:
	GPGPUMatrixVectorMathApp(int width, int height);
	~GPGPUMatrixVectorMathApp();

	void Initialize();
	void Run();
	void Terminate();
    
    void InitializeOpenCL();
    void TerminateOpenCL();

	// Event handlers.
	void OnKeyboard(unsigned char key, int x, int y);
	void OnKeyboardUp(unsigned char key, int x, int y);
	void OnMouse(int button, int state, int x, int y);
	void OnMouseMove(int x, int y);
	void OnReshape(int x, int y);

private:
	std::string mWindowTitle;
    
    ComputeProgramPtr mProgram;
    ComputeKernelPtr mKernel;
    CommandQueuePtr mCommandQueue;
    MemoryObjectPtr mMatrix;
    MemoryObjectPtr mVector;
    MemoryObjectPtr mResult;
};

}

#endif