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
	mWindow = glfwCreateWindow(1024, 768, mTitle.c_str(), NULL, NULL);
	if (mWindow == NULL)
	{
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n");
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(mWindow);

	// Initialize GLEW
	//glewExperimental = true; // set to true for core profile
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		return;
	}

	// default GL settings
	float color = 0.5f;
	glClearColor(color, color, color, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Call child class initialize
	this->Initialize(device);
	this->mInitialized = true;
}
//----------------------------------------------------------------------------
void Application::UpdateMainCamera()
{
	static auto lastFrame = 0.0;
	auto currentFrame = glfwGetTime();
	float deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	static auto lastMousePos = glm::vec2(-1, -1);
	static auto mouseStartPos = glm::vec2(0, 0);
	const float mouseSpeed = 0.003f;

	auto camDir = mMainCamera->GetDirection();
	auto camPos = mMainCamera->GetLocation();
	auto camSpeed = 5.0f;
	static float horizontalAngle = 0;
	static float verticalAngle = 0;

	// Get mouse position
	auto newMousePos = glm::dvec2();
	glfwGetCursorPos(mWindow, &newMousePos.x, &newMousePos.y);

	// Camera Orientation
	// If left click
	if (glfwGetMouseButton(mWindow, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		// proceed only if left button is just clicked
		if (lastMousePos == glm::vec2(-1, -1))
		{
			// Hide cursor
			glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
			// Just clicked, do nothing but record initial values
			mouseStartPos = newMousePos;
			lastMousePos = newMousePos;
		}
		else
		{
			// Calculate camera direction with mouse displacement
			horizontalAngle += mouseSpeed * float(mouseStartPos.x - newMousePos.x);
			verticalAngle -= mouseSpeed * float(mouseStartPos.y - newMousePos.y);
			// Reset mouse position for next frame
			glfwSetCursorPos(mWindow, mouseStartPos.x, mouseStartPos.y);
		}
	}
	else
	{
		lastMousePos = glm::vec2(-1, -1);
		// Show the cursor
		glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	mMainCamera->SetAngle(horizontalAngle, verticalAngle);
	auto rightDir = mMainCamera->GetRight();

	// Camera Position
	// Move forward
	if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS ||
		glfwGetKey(mWindow, GLFW_KEY_W) == GLFW_PRESS)
	{
		camPos -= camDir * deltaTime * camSpeed;
	}
	// Move backward
	if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS ||
		glfwGetKey(mWindow, GLFW_KEY_S) == GLFW_PRESS)
	{
		camPos += camDir * deltaTime * camSpeed;
	}
	// Move up
	if (glfwGetKey(mWindow, GLFW_KEY_E) == GLFW_PRESS)
	{
		camPos.y += deltaTime * camSpeed;
	}
	// Move down
	if (glfwGetKey(mWindow, GLFW_KEY_Q) == GLFW_PRESS)
	{
		camPos.y -= deltaTime * camSpeed;
	}
	// Strafe right
	if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS ||
		glfwGetKey(mWindow, GLFW_KEY_D) == GLFW_PRESS)
	{
		camPos += rightDir * deltaTime * camSpeed;
	}
	// Strafe left
	if (glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_PRESS ||
		glfwGetKey(mWindow, GLFW_KEY_A) == GLFW_PRESS)
	{
		camPos -= rightDir * deltaTime * camSpeed;
	}

	mMainCamera->SetLocation(camPos);
}
//----------------------------------------------------------------------------
void Application::ProcessInput()
{
	glfwGetWindowSize(mWindow, &mWidth, &mHeight);
	UpdateMainCamera();
	if (glfwGetKey(mWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(mWindow, true);
	}
	this->ProcessInput();
}
//----------------------------------------------------------------------------
void Application::Run()
{
	while (!glfwWindowShouldClose(mWindow))
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
	glfwSwapBuffers(mWindow);
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
