#include "SSDOApp.h"
#include "OpenGLDevice.h"

using namespace RTGI;

Application* app = 0;

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
	// init glut
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	int width = 1024;
	int height = 768;
    glutInitWindowSize(width, height);

	// create GLUT window for drawing
    glutCreateWindow(0);

	// init glew
#ifndef __APPLE__
    glewInit();
#endif

    // Create GPU device.
    OpenGLDevice* gpuDevice = new OpenGLDevice();

	// Initialize application.
	app = new SSDOApp(width, height);
    app->Initialize(gpuDevice);

	// assign handlers
    glutDisplayFunc(OnIdle);
    glutKeyboardFunc(OnKeyboard);
	glutKeyboardUpFunc(OnKeyboardUp);
	glutReshapeFunc(OnReshape);
	glutMouseFunc(OnMouse);
	glutMotionFunc(OnMouseMove);

	// enter the drawing loop
	// frame rate can be controlled with 
    glutMainLoop();

	// Terminate application.
	app->Terminate();
	delete app;

    return 0;
}
//----------------------------------------------------------------------------