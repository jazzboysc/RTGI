#include "SSDOApp.h"
#include "OpenGLDevice.h"

using namespace RTGI;

Application* app = 0;

//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    // Create GPU device.
    OpenGLDevice* gpuDevice = new OpenGLDevice();

	// Initialize application.
	app = new SSDOApp(1024, 768);
	app->Application::Initialize(gpuDevice);
	app->Application::Run();
	app->Application::Terminate();
	delete app;

    return 0;
}
//----------------------------------------------------------------------------