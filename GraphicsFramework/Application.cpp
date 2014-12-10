//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Application.h"
#include <sys/timeb.h>

using namespace RTGI;

Application* Application::mInstance = 0;

//----------------------------------------------------------------------------
Application::Application()
{
#ifdef _WIN32
	mOpenGLContext = wglGetCurrentContext();
	mWindowsDeviceContext = wglGetCurrentDC();
#else
#ifdef __APPLE__
    mOpenGLContext = CGLGetCurrentContext();
	mShareGroup = CGLGetShareGroup(mOpenGLContext);
#endif
#endif

    assert( mInstance == 0 );
    mInstance = this;
}
//----------------------------------------------------------------------------
Application::~Application()
{
    mDevice = 0;
}
//----------------------------------------------------------------------------
void Application::Initialize(GPUDevice* device)
{
    mDevice = device;
}
//----------------------------------------------------------------------------
void Application::Run()
{
}
//----------------------------------------------------------------------------
void Application::Terminate()
{
}
//----------------------------------------------------------------------------
void Application::OnKeyboard(unsigned char, int, int)
{
}
//----------------------------------------------------------------------------
void Application::OnKeyboardUp(unsigned char, int, int)
{
}
//----------------------------------------------------------------------------
void Application::OnMouse(int, int, int, int)
{
}
//----------------------------------------------------------------------------
void Application::OnMouseMove(int, int)
{
}
//----------------------------------------------------------------------------
void Application::OnReshape(int, int)
{
}
//----------------------------------------------------------------------------
Application* Application::GetInstance()
{
    return mInstance;
}
//----------------------------------------------------------------------------