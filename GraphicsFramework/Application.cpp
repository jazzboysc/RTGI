//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Application.h"
#include <sys/timeb.h>

using namespace RTGI;

//----------------------------------------------------------------------------
Application::Application()
{
#ifdef _WIN32
	mOpenGLContext = wglGetCurrentContext();
#else
#ifdef __APPLE__
    mOpenGLContext = CGLGetCurrentContext();
#endif
#endif
}
//----------------------------------------------------------------------------
Application::~Application()
{
}
//----------------------------------------------------------------------------
void Application::Initialize()
{
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