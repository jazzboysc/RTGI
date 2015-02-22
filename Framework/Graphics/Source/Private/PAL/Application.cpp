//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

//TODO: it is technically windows appication right now
#include "Application.h"
#include <sys/timeb.h>
#include <glfw3.h>

using namespace RTGI;

Application* Application::mInstance = 0;

//----------------------------------------------------------------------------
Application::Application()
{
	mOpenGLContext = wglGetCurrentContext();
	mWindowsDeviceContext = wglGetCurrentDC();

    assert( mInstance == 0 );
    mInstance = this;

}
//----------------------------------------------------------------------------
Application::~Application()
{
	mDevice->Terminate();
    mDevice = 0;
}
//----------------------------------------------------------------------------
void Application::Initialize(GPUDevice* device)
{
    mDevice = device;
	mMainCamera = new Camera;

	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		exit(EXIT_FAILURE);
	}

	// TODO: get MSAA from device?
	glfwWindowHint(GLFW_SAMPLES, 4);

	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	Window = glfwCreateWindow(Width, Height, Title.c_str(), NULL, NULL);
	if (Window == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(Window);
	glfwSetKeyCallback(Window, Application::KeyboardCallbackWrapper);
	// Initialize GLEW
	//glewExperimental = true; // set to true for core profile
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return;
	}

	// TODO: should init GL context but we are using GLFW
    mDevice->Initialize(0);

	// Anisotropic Filtering
	GLint maxAnisFilterLevel;
	mDevice->GetMaxAnisFilterLevel(&maxAnisFilterLevel);
	mDevice->SetAnisFilterLevel(maxAnisFilterLevel);

	// some GL default values
	float color = 0.0f;
	glClearColor(color, color, color, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    // Unrestricted frame rates.
    glfwSwapInterval(0);

	// Call child class initialize
	this->Initialize(device);
	this->mInitialized = true;
}
//----------------------------------------------------------------------------
void Application::UpdateMainCamera()
{
	static auto lastFrame = 0.0;
	auto currentFrame = glfwGetTime();
	float deltaTime = float(currentFrame - lastFrame);
	lastFrame = currentFrame;

	static auto lastMousePos = glm::vec2(-1, -1);
	static auto mouseStartPos = glm::vec2(0, 0);
	const float mouseSpeed = 0.15f;

	auto camDir = mMainCamera->GetDirection();
	auto camPos = mMainCamera->GetLocation();
	auto camSpeed = 5.0f;
	auto horizontalAngle = 0.f;
	auto verticalAngle = 0.f;
	mMainCamera->GetAngle(horizontalAngle, verticalAngle);

	// Get mouse position
	auto newMousePos = glm::dvec2();
	glfwGetCursorPos(Window, &newMousePos.x, &newMousePos.y);

	// Camera Orientation
	// If left click
	if (glfwGetMouseButton(Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// proceed only if left button is just clicked
		if (lastMousePos == glm::vec2(-1, -1))
		{
			// Hide cursor
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			// Just clicked, do nothing but record initial values
			mouseStartPos = newMousePos;
			lastMousePos = newMousePos;
		}
		else
		{
			// Calculate camera direction with mouse displacement
			auto h = mouseSpeed * float(newMousePos.x - mouseStartPos.x);
			auto v = mouseSpeed * float(newMousePos.y - mouseStartPos.y);
			auto rot = glm::vec3(h, v, 0);
			mMainCamera->Rotate(rot);
			// Reset mouse position for next frame
			glfwSetCursorPos(Window, mouseStartPos.x, mouseStartPos.y);
		}
	}
	else
	{
		lastMousePos = glm::vec2(-1, -1);
		// Show the cursor
		glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	auto rightDir = mMainCamera->GetRight();

	// Camera Position
	// Move forward
	if (glfwGetKey(Window, GLFW_KEY_UP) == GLFW_PRESS ||
		glfwGetKey(Window, GLFW_KEY_W) == GLFW_PRESS)
	{
		camPos -= camDir * deltaTime * camSpeed;
	}
	// Move backward
	if (glfwGetKey(Window, GLFW_KEY_DOWN) == GLFW_PRESS ||
		glfwGetKey(Window, GLFW_KEY_S) == GLFW_PRESS)
	{
		camPos += camDir * deltaTime * camSpeed;
	}
	// Move up
	if (glfwGetKey(Window, GLFW_KEY_E) == GLFW_PRESS)
	{
		camPos.y += deltaTime * camSpeed;
	}
	// Move down
	if (glfwGetKey(Window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camPos.y -= deltaTime * camSpeed;
	}
	// Strafe right
	if (glfwGetKey(Window, GLFW_KEY_RIGHT) == GLFW_PRESS ||
		glfwGetKey(Window, GLFW_KEY_D) == GLFW_PRESS)
	{
		camPos += rightDir * deltaTime * camSpeed;
	}
	// Strafe left
	if (glfwGetKey(Window, GLFW_KEY_LEFT) == GLFW_PRESS ||
		glfwGetKey(Window, GLFW_KEY_A) == GLFW_PRESS)
	{
		camPos -= rightDir * deltaTime * camSpeed;
	}

	mMainCamera->SetLocation(camPos);
}
//----------------------------------------------------------------------------
void Application::ProcessInput()
{
	glfwGetWindowSize(Window, &Width, &Height);
	UpdateMainCamera();
	if (glfwGetKey(Window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(Window, true);
	}
	this->ProcessInput();
}
//----------------------------------------------------------------------------
void Application::Run()
{
	while (!glfwWindowShouldClose(Window))
	{
		this->Application::ProcessInput();
		this->Application::FrameFunc();
	}
}
//----------------------------------------------------------------------------
void Application::FrameFunc()
{
	this->FrameFunc();
	// Swap buffers
	glfwSwapBuffers(Window);
	glfwPollEvents();
}
//----------------------------------------------------------------------------
void Application::Terminate()
{
	this->Terminate();
	delete mMainCamera;
	mMainCamera = nullptr;
	glfwTerminate();
}
//----------------------------------------------------------------------------
Application* Application::GetInstance()
{
    return mInstance;
}
//----------------------------------------------------------------------------
void Application::KeyboardCallbackWrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//mInstance->Application::ProcessInput(key, scancode, action, mods);
}
//----------------------------------------------------------------------------
void Application::ProcessInput(int key, int scancode, int action, int mods)
{

}