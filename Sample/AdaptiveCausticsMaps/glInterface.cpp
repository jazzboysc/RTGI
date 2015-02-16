/***************************************************************************/
/* glInterface.cpp                                                         */
/* -----------------------                                                 */
/*                                                                         */
/* Basic GLUT callbacks for the sceneLoader OpenGL program.                */
/*                                                                         */
/* Chris Wyman (02/01/2008)                                                */
/***************************************************************************/

#include "causticDemo.h"
#include "RenderingTechniques/renderingData.h"
#include "RenderingTechniques/renderingMethods.h"
#include "Scene/Scene.h"
#include "Interface/SceneFileDefinedInteraction.h"
#include "Utils/HighResolutionTimer.h"

extern Trackball *eyeBall;
extern Trackball *lightBall[4];
extern Trackball *objBall[4];

extern Scene *scene;
extern SceneDefinedUI *ui;
extern RenderingData *data;
extern FrameGrab *frameGrab;
extern MovieMaker *movieGrab;
extern FrameRate *frameSpeed;
double currentTime = -1;
bool firstTime = true;
TimerStruct lastTime, curTime; 

// Updates to the scene based on an animation?
void IdleCallback ( void )
{
	// If the user changed # lights enabled, enable/disable them
	if (*data->ui->numLightsUsed > scene->NumLightsEnabled())
		for (int i=0; i < *data->ui->numLightsUsed; i++)
			scene->GetLight( i )->Enable();
	else if (*data->ui->numLightsUsed < scene->NumLightsEnabled())
	{
		for (int i=0; i < scene->NumLightsEnabled(); i++)
		{
			if (i <*data->ui->numLightsUsed) scene->GetLight( i )->Enable();
			else scene->GetLight( i )->Disable();
		}
	}

	if (firstTime)
	{
		GetHighResolutionTime( &lastTime );
		firstTime = false;
		currentTime = 0;
	}
	else if (*data->ui->animation)
	{
		GetHighResolutionTime( &curTime );
		scene->IncrementFrameTime( ConvertTimeDifferenceToSec( &curTime, &lastTime ) );
		lastTime = curTime;
	}
	else if (!*data->ui->animation)
		GetHighResolutionTime( &lastTime );
	if (data->param->lightFOV)
		scene->GetLight( 0 )->SetFOV( *data->param->lightFOV );
	scene->PerFrameUpdate( );
	glutPostRedisplay();
}


// This is where most changes occurring from UI events occur.  This uses
//   UI definitions from a scene file, as specified by the SceneDefinedUI
//   class, which allows a file to map keys to functionality directly.
// The mouse, keyboard, special_keys callbacks below all call this function.
void ApplyUICommand( unsigned int curCommand, int xVal, int yVal )
{
	if ( curCommand == KEY_UNKNOWN ) return;
	else if ( curCommand == ui->Key( UI_RELOAD_SHADERS ) )
	{
		printf("(+) Reloading Shaders....");
		bool ok = scene->ReloadShaders();
		printf( ok ? "Successful!\n" : "Errors encountered!" );
		glutPostRedisplay();
	}
	else if ( curCommand == ui->Key( UI_SCREEN_CAPTURE ) )
	{
		char nextFile[512];
		frameGrab->GetFilenameForNextFrame( nextFile, 512 );
		data->ui->captureScreen = true;
		printf("(+) Capturing next frame to '%s'.\n", nextFile );
		glutPostRedisplay();
	}
	else if ( curCommand == ui->Key( UI_QUIT ) )
	{
		FreeMemoryAndQuit();
	}
	else if ( curCommand == ui->Key( UI_HELPSCREEN ) )
	{
		data->ui->displayHelp = !data->ui->displayHelp;
		glutPostRedisplay();
	}

}


void ReshapeCallback ( int w, int h )
{
	int newWidth = w, newHeight = h;

	// Double check that we're willing to allow the screen resolution to change...
	
	//       (Yes, I guess...)

	// Setup the new projection matrix and viewport
	glViewport( 0, 0, newWidth, newHeight );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();
	scene->PerspectiveMatrix();
	glMatrixMode( GL_MODELVIEW );
	glutPostRedisplay();

	// Resize the trackballs, if necessary.
	if (eyeBall) eyeBall->ResizeTrackballWindow( newWidth, newHeight );
	for (int i=0; i<4; i++)
	{
		if (lightBall[i]) lightBall[i]->ResizeTrackballWindow( newWidth, newHeight );
		if (objBall[i])   objBall[i]->ResizeTrackballWindow( newWidth, newHeight );
	}
}

