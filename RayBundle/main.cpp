#include "RayBundleApp.h"

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
	int width = 256;
	int height = 256;
    glutInitWindowSize(width, height);

    // If you are using freeglut, the next two lines will check if 
    // the code is truly 3.2. Otherwise, comment them out
    
    //glutInitContextVersion(3, 3);
    //glutInitContextProfile(GLUT_CORE_PROFILE);

	// create GLUT window for drawing
    glutCreateWindow(0);

	// init glew
#ifndef __APPLE__
    glewInit();
#endif

	// Initialize application.
	app = new RayBundleApp(width, height);
	app->Initialize();

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