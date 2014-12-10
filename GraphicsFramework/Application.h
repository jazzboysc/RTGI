//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Application_H
#define RTGI_Application_H

#include "FrameworkCommon.h"
#include "GPUDevice.h"

#ifdef _WIN32
#include <wingdi.h>
#endif

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/14/2013
//----------------------------------------------------------------------------
class Application
{
public:
	Application();
	virtual ~Application();

	virtual void Initialize(GPUDevice* device);
	virtual void Run();
	virtual void Terminate();

	virtual void OnKeyboard(unsigned char key, int x, int y);
	virtual void OnKeyboardUp(unsigned char key, int x, int y);
	virtual void OnMouse(int button, int state, int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnReshape(int x, int y);

    static Application* GetInstance();
    
protected:
    GPUDevicePtr mDevice;

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