#include "OpenGLDevice.h"

using namespace RTGI;

Application* app = 0;
//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    // Create GPU device.
    OpenGLDevice* gpuDevice = new OpenGLDevice();

	// Initialize application.
	app = new Application();
	app->RTGI::Application::Initialize(gpuDevice);

	auto pInfo = ShaderProgramInfo();
	ShaderProgramInfo SICausticsMapIntersect;
	SICausticsMapIntersect << "Caustics/Intersection.vert" << "Caustics/Intersection.frag";
	auto pShaderProgram = new ShaderProgram(SICausticsMapIntersect);

	// Test b
	//(gpuDevice->*(gpuDevice->CreateProgram))(pShaderProgram);
	delete app;

	//GPU_DEVICE_FUNC(gpuDevice, CreateProgram)(pShaderProgram);
	//(*gpuDevice.*(gpuDevice->CreateProgram))(pShaderProgram);
	//(*this.*pt2Member)(12, 'a', 'b');
    return 0;
}
//----------------------------------------------------------------------------