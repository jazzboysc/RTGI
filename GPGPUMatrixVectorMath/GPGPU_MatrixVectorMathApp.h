#ifndef RTGI_GPGPU_MatrixVectorMathApp_H
#define RTGI_GPGPU_MatrixVectorMathApp_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/11/2014
//----------------------------------------------------------------------------
class GPGPU_MatrixVectorMathApp : public Application
{
public:
	GPGPU_MatrixVectorMathApp(int width, int height);
	~GPGPU_MatrixVectorMathApp();

	void Initialize();
	void Run();
	void Terminate();

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