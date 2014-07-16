#ifndef RTGI_GPGPUPostProcessingApp_H
#define RTGI_GPGPUPostProcessingApp_H

#include "GraphicsFrameworkHeader.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/15/2014
//----------------------------------------------------------------------------
class GPGPUPostProcessingApp : public Application
{
public:
	GPGPUPostProcessingApp(int width, int height);
	~GPGPUPostProcessingApp();

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
	int mWidth, mHeight;

	Camera* mCamera;

	FrameBufferPtr mRenderBuffer;
	Texture2DPtr mRenderTexture;
    
    ComputeProgramPtr mProgram;
    ComputeKernelPtr mKernel;
    CommandQueuePtr mCommandQueue;
    MemoryObjectPtr mMatrix;
    MemoryObjectPtr mVector;
    MemoryObjectPtr mResult;
};

}

#endif