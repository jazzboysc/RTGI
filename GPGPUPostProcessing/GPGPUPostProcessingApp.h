#ifndef RTGI_GPGPUPostProcessingApp_H
#define RTGI_GPGPUPostProcessingApp_H

#include "GraphicsFrameworkHeader.h"
#include "GPGPUPostProcessingTriMesh.h"
#include "GPGPUPostProcessingScreenQuad.h"

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
    
    void InitializeOpenCL();
    void TerminateOpenCL();

	// Event handlers.
	void OnKeyboard(unsigned char key, int x, int y);
	void OnKeyboardUp(unsigned char key, int x, int y);
	void OnMouse(int button, int state, int x, int y);
	void OnMouseMove(int x, int y);
	void OnReshape(int x, int y);

private:
	int mWidth, mHeight;    
	std::string mWindowTitle;

	Camera* mCamera;

	FrameBufferPtr mRenderBuffer;
	Texture2DPtr mRenderTexture;
    Texture2DPtr mDepthTexture;
    
    GPGPUPostProcessingTriMeshPtr mModel;
    GPGPUPostProcessingScreenQuadPtr mScreenQuad;

    // GPGPU stuff
    ComputeProgramPtr mProgram;
    ComputeKernelPtr mKernel;
    CommandQueuePtr mCommandQueue;
    MemoryObjectPtr mKernelRenderTexture;
};

}

#endif