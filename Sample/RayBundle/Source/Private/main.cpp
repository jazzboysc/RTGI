#include "RayBundleApp.h"
#include "OpenGLDevice.h"

using namespace RTGI;

Application* app = 0;
//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	// Create GPU device.
	OpenGLDevice* gpuDevice = new OpenGLDevice();

	// Initialize application.
	app = new RayBundleApp();
	app->RTGI::Application::Initialize(gpuDevice);
	app->RTGI::Application::Run();
	app->RTGI::Application::Terminate();
	delete app;

	return 0;
}
//----------------------------------------------------------------------------