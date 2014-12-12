#include "BidirectionalVoxelGIApp.h"
#include "GUIFramework.h"
#include "OpenGLDevice.h"

using namespace RTGI;
using namespace RTGI::GUIFramework;

RTGI::Application* app = 0;

//----------------------------------------------------------------------------
void OnIdle()
{
	app->Run();
	glutPostRedisplay();
}
//----------------------------------------------------------------------------
void OnKeyboard(unsigned char key, int x, int y)
{
    switch( key )
	{
    case 033:
        exit(EXIT_SUCCESS);
        break;
    }

	app->OnKeyboard(key, x, y);
}
//----------------------------------------------------------------------------
void OnKeyboardUp(unsigned char key, int x, int y)
{
	app->OnKeyboardUp(key, x, y);
}
//----------------------------------------------------------------------------
void OnMouse(int button, int state, int x, int y)
{
	app->OnMouse(button, state, x, y);
}
//----------------------------------------------------------------------------
void OnMouseMove(int x, int y)
{
	app->OnMouseMove(x, y);
}
//----------------------------------------------------------------------------
void OnReshape(int x, int y)
{
	app->OnReshape(x, y);
}
//----------------------------------------------------------------------------
int main(int argc, char **argv)
{
	// Init glut.
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	int width = 768;
	int height = 768;
    glutInitWindowSize(width, height);

	// Create GLUT window for drawing.
    glutCreateWindow(0);

	// Init glew.
    glewInit();

    // Create information panel.
    int screenX = glutGet(GLUT_WINDOW_X);
    int screenY = glutGet(GLUT_WINDOW_Y);
    InformationPanel^ infoPanel = gcnew InformationPanel();
    infoPanel->Show();
    infoPanel->SetDesktopLocation(screenX + width + 12, screenY - 30);

    // Create GPU device.
    OpenGLDevice* gpuDevice = new OpenGLDevice();

    // Initialize application.
    app = new BidirectionalVoxelGIApp(width, height);
    app->Initialize(gpuDevice);

	// Assign handlers.
    glutDisplayFunc(OnIdle);
    glutKeyboardFunc(OnKeyboard);
	glutKeyboardUpFunc(OnKeyboardUp);
	glutReshapeFunc(OnReshape);
	glutMouseFunc(OnMouse);
	glutMotionFunc(OnMouseMove);

	// Enter the drawing loop.
    glutMainLoop();

	// Terminate application.
	app->Terminate();
	delete app;

    return 0;
}
//----------------------------------------------------------------------------