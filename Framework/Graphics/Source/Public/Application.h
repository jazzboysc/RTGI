//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Application_H
#define RTGI_Application_H

#include "GPUDevice.h"
// TODO split window specific impl into windowsApplication class
#define WIN32_LEAN_AND_MEAN 1
#include <windows.h>
struct GLFWwindow;

namespace RTGI
{
//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class Camera;
class Application
{
public:
	Application();
	virtual ~Application();

	virtual void Initialize(GPUDevice* device);
	virtual void Run();
	virtual void Terminate();
	virtual void ProcessInput(int key, int scancode, int action, int mods);
	static void KeyboardCallbackWrapper(
		GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void FrameFunc();
	void UpdateMainCamera();

	static Application* GetInstance();

	GLFWwindow* Window;
	int Width, Height;
	std::string Title = "Default Application";

protected:
	GPUDevicePtr mDevice;
	Camera* mMainCamera;
	bool mInitialized = false;

#ifdef _WIN32
	HGLRC mOpenGLContext;
	HDC mWindowsDeviceContext;
#else
#ifdef __APPLE__
	CGLContextObj mOpenGLContext;
	CGLShareGroupObj mShareGroup;
#endif
#endif

	static Application* mInstance;
};

}

#endif