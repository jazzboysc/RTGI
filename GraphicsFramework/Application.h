//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Application_H
#define RTGI_Application_H

#include "FrameworkCommon.h"

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

	virtual void Initialize();
	virtual void Run();
	virtual void Terminate();

	virtual void OnKeyboard(unsigned char key, int x, int y);
	virtual void OnKeyboardUp(unsigned char key, int x, int y);
	virtual void OnMouse(int button, int state, int x, int y);
	virtual void OnMouseMove(int x, int y);
	virtual void OnReshape(int x, int y);

    void InitializeOpenCL();
    void TerminateOpenCL();
    
protected:
#ifdef _WIN32
	HGLRC mOpenGLContext;
#else
#ifdef __APPLE__
    CGLContextObj mOpenGLContext;
#endif
#endif
    
    cl_platform_id mOpenCLPlatform;
    cl_device_id mOpenCLDevice;
    cl_context mOpenCLContext;
};

}

#endif