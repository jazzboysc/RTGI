#include "SVOApp.h"
#include "GUIFramework.h"
#include "OpenGLDevice.h"
#include <glfw3.h>

using namespace RTGI;
using namespace RTGI::GUIFramework;

RTGI::Application* app = 0;
//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	// Create GPU device.
	OpenGLDevice* gpuDevice = new OpenGLDevice();

	// Initialize application.
    app = new SVOApp(768, 768);
	app->RTGI::Application::Initialize(gpuDevice);
	app->RTGI::Application::Run();
	app->RTGI::Application::Terminate();
	delete app;

	return 0;
}
//----------------------------------------------------------------------------