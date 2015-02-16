/***************************************************************************/
/* sceneLoader.cpp                                                         */
/* -----------------------                                                 */
/*                                                                         */
/* This is the main entry point into my current OpenGL framework (entitled */
/*    simply enough "sceneLoader").  This code parses command-line params, */
/*    initializes the scene, initializes OpenGL, checks for all hardware   */
/*    functionality & OpenGL driver support needed for the demo, and fires */
/*    of the main GLUT window.  It also contain a basic GLUT display       */
/*    callback, which checks to see which render mode we're currently set  */
/*    to use.  Except for a simplistic diffuse rendering of the scene, all */
/*    code for various rendering modes are in separate files inside the    */
/*    directory "RenderingTechniques/"                                     */
/*                                                                         */
/* Chris Wyman (02/01/2008)                                                */
/***************************************************************************/

#include "causticDemo.h"
#include "RenderingTechniques/renderingData.h"
#include "RenderingTechniques/renderingMethods.h"

Scene *scene = 0;
FrameRate *frameSpeed = 0;
FrameRate *frameSpeedSlow = 0;
FrameGrab *frameGrab  = 0;
extern RenderingData *data;

int *displayID=0, lastDisplay=-1;

char *GetWindowName( int identifier )
{
    #define NUM_TITLES   6
	static char buf[NUM_TITLES][128] = {
		"Adaptive Caustic Maps Demo -- (1) Basic, Diffuse Scene Rendering",
		"Adaptive Caustic Maps Demo -- (2) Deferred Image-Space Refraction",
		"Adaptive Caustic Maps Demo -- (3) Caustics Via Image-Space Splatting (Wyman & Davis 2006)",
		"Adaptive Caustic Maps Demo -- (4) Adaptive Caustics Via Image-Space Splatting (Maximal Traversal Metric)",
		"Adaptive Caustic Maps Demo -- (5) Hierarchical Caustic Maps (Wyman 2008)",
		"Adaptive Caustic Maps Demo -- (6) Adaptive Hierarchical Caustic Maps (Maximal Caustic Map Error) ",
	};
	static char newTitle[256];
	sprintf( newTitle, "Error!  Unknown title text!!!  (Id: %d)", identifier );
	return identifier < 0 || identifier >= NUM_TITLES ? newTitle : buf[identifier];
	#undef NUM_TITLES
}


void DisplayCallback( void )
{
	// Make sure the title of the window stays up-to-date if we change render modes
	if (*displayID != lastDisplay)
	{
		glutSetWindowTitle( GetWindowName( *displayID ) );
		lastDisplay = *displayID;
	}

	// Start timing this frame
	frameSpeed->StartFrame();
	frameSpeedSlow->StartFrame();

	// Draw the scene using the appropriate render mode
	if (*displayID > 0)
	{
		switch( *displayID )
		{
		case 1:
			Display_WithDeferredRefraction();
			break;
		case 2:
			Display_WithOriginalRefractiveCaustics();
			break;
		case 3:
			Display_WithAdaptiveCaustics();
			break;
		case 4:
			Display_WithOriginalHierarchicalCaustics();
			break;
		case 5:
			Display_WithAdaptiveCausticsUsingErrorMetricAndBatching();
			break;
		}
	}
	else  // This is the render mode for a simple, diffuse scene
	{
		// Create shadow map(s) 
		for (int i=0; i < scene->NumLightsEnabled(); i++)
			scene->CreateShadowMap( data->fbo->shadowMap[i], 
									data->param->shadowMatrixTrans[i], 
									i, // Light Number
									*(data->param->shadowMapBias) );

		// Draw the scene with shadowing enabled
		data->fbo->mainWin->BindBuffer();
		data->fbo->mainWin->ClearBuffers();
		glLoadIdentity();
		scene->LookAtMatrix();
		scene->SetupEnabledLightsWithCurrentModelview();
		scene->Draw( MATL_FLAGS_USESHADOWMAP );
		data->fbo->mainWin->UnbindBuffer();
	}

	// Copy the final rendering to the screen
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	data->fbo->mainWin->DisplayAsFullScreenTexture( FBO_COLOR0 );

	// If a screen capture is desired, do it now (before writing timer text)
	if (data->ui->captureScreen)
	{
		frameGrab->CaptureFrame();
		data->ui->captureScreen = false;
	}

	// If the user wants the help screen then display it.
	if (data->ui->displayHelp)
	{
		if (data->ui->updateHelpScreen)
			SetupHelpScreen( data->fbo->helpScreen );
		DisplayHelpScreen( data->fbo->helpScreen );
	}

	// Output the timer text
	float slowSpeed = frameSpeedSlow->EndFrame();
	if (slowSpeed < 10)
	{
		DisplayTimer( slowSpeed, 1024, 1024 );
		frameSpeed->EndFrame();
	}
	else
		DisplayTimer( frameSpeed->EndFrame(), 1024, 1024 );
	glFlush();
	glutSwapBuffers();
}


// Check to make sure whoever runs the demo has a capable card...
void CheckMachineCapabilities( void )
{
	printf("(+) Checking for necessary hardware requirements.....");

	if ( GLEW_VERSION_2_0 && 
		 GLEW_EXT_gpu_shader4 && 
		 GLEW_EXT_geometry_shader4 &&
		 GLEW_NV_transform_feedback &&
		 GLEW_EXT_texture_array )
	{
		printf("  OK.\n");
		return;
	}
	else printf("  MISSING!\n");

	printf("    (-) Running: OpenGL v%s, GLEW v%s, GLSL v%s \n", 
		glGetString( GL_VERSION ), 
		glewGetString( GLEW_VERSION ),
		glGetString(GL_SHADING_LANGUAGE_VERSION) );
	printf("    (-) Compiled against: GLUT v3.7.6 \n");
	printf("    (-) Running on a: %s %s\n", glGetString( GL_VENDOR ), glGetString( GL_RENDERER ));
	glGetError();
	
	/* check if the machine is equipped to run the demo */
	if (!GLEW_VERSION_2_0)
		printf("    (-) FATAL ERROR: Demo requires OpenGL 2.0 or higher!\n");
	if (!GLEW_EXT_gpu_shader4)
		printf("    (-) FATAL ERROR: Demo requires OpenGL Extension EXT_gpu_shader4!\n");
	if (!GLEW_EXT_geometry_shader4)
		printf("    (-) FATAL ERROR: Demo requires OpenGL Extension EXT_geometry_shader4!\n");
	if (!GLEW_EXT_texture_array)
		printf("    (-) FATAL ERROR: Demo requires OpenGL Extension EXT_texture_array!\n");
	if (!GLEW_NV_transform_feedback)
	{
		printf("    (-) FATAL ERROR: Demo requires OpenGL Extension NV_transform_feedback!\n");
		printf("    (-) NOTE: This demo probably can be converted to EXT_transform_feedback.\n");
		printf("              However, as of early 2009, I do not have hardware and drivers\n");
		printf("              to test this.\n");
	}

	// Make sure we don't quickly close the program without letting
	//    the user read about their horribly deficient hardware!
	printf("(+) Press enter to exit...\n" );
	getchar();

	// OK.  Now quit.
	exit(-1);
}


// The main entry point.
int main(int argc, char* argv[] )
{
	bool verbose = false;
	char scenefile[ 256 ];
	printf("**************************************************************************\n");
	printf("*             Adaptive Caustic Maps Using Deferred Shading               *\n");
    printf("*   (Computer Graphics Forum 28(2) & Proceedings of Eurographics 2009)   *\n");
    printf("*                     Chris Wyman and Greg Nichols                       *\n");
	printf("*                          University of Iowa                            *\n");
	printf("*    More Information: http://www.cs.uiowa.edu/~cwyman/publications/     *\n");
	printf("**************************************************************************\n");

	// Check to make sure the user gave a scene file.  If not, prompt for one.
	printf("(+) Parsing any command line parameters...\n");
	if (argc < 2)
	{
		printf("\nUsage: causticDemo.exe <sceneFile>\n\n", argv[0]);
		printf("    (-) No scene file specified!  Which scene would you like?\n");
		printf("        1) buddhaCaustics.txt  (allows 2 lights)\n");
		printf("        2) dragonCaustics.txt    \n");
		printf("        3) elephantCaustics.txt  \n");
		printf("        4) felineCaustics.txt    \n");
		printf("        5) yeahRightCaustics.txt (uses textured light)\n");
		printf("    (-) Enter [1-5]: "); fflush( stdout );
		int val = 1;
		char buf[16];
		fgets( buf, 15, stdin );
		sscanf( buf, "%d", &val);
		switch( val )
		{
		default:
			sprintf( scenefile, "buddhaCaustics.txt" );
			break;
		case 2:
			sprintf( scenefile, "dragonCaustics.txt" );
			break;
		case 3:
			sprintf( scenefile, "elephantCaustics.txt" );
			break;
		case 4:
			sprintf( scenefile, "felineCaustics.txt" );
			break;
		case 5:
			sprintf( scenefile, "yeahRightCaustics.txt" );
			break;
		}
	}
	else  // Grab the scene from the command line.
	{
		for( int i=1; i < argc; i++ )
		{
			if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help") || 
				!strcmp(argv[i], "-?") || !strcmp(argv[i], "/?") )
			{
				printf("Usage: %s <sceneFile>\n", argv[0]);
				exit(0);
			}
			else if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "-verbose"))
				verbose = true;
			else
			{
				strncpy( scenefile, argv[i], 255 );
				break;
			}
		}
	}

	// Load the scene from the file.  Unfortunately, this needs to come prior
	//   to initializing the GL window and checking for an appropriate hardware
	//   configuration.  It should really be the other way around for a demo...
	scene = new Scene( scenefile, verbose );


	printf("(+) Initializing OpenGL state...\n");
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_ALPHA );
	glutInitWindowSize( scene->GetWidth(), scene->GetHeight() );
	glutCreateWindow( "DEMO: Adaptive Caustic Maps Using Deferred Shading" );
	glewInit();

	// Check hardware compatibility... before we crash from NULL GLEW pointers.
	CheckMachineCapabilities();

	// Set the GLUT callback functions 
	glutDisplayFunc( DisplayCallback );
	glutReshapeFunc( ReshapeCallback );
	glutIdleFunc( IdleCallback );
	glutMouseFunc( MouseButtonCallback );
	glutMotionFunc( MouseMotionCallback );
	glutKeyboardFunc( KeyboardCallback );
	glutSpecialFunc( SpecialKeyboardCallback ); 

	// Other program setup 
	frameSpeed = new FrameRate( 30 );
	frameSpeedSlow = new FrameRate( 5 );
	frameGrab  = new FrameGrab();

	// Make sure any preprocessing that needs to be done occurs
	scene->Preprocess();

	// Initialized data needed by the various rendering modes
	InitializeRenderingData();

	if (scene->IsVerbose())
		printf("(+) back in main()...\n");

	// Get a pointer to the Display ID
	displayID = scene->GetSceneIntVar( "displayid" );
	if (!displayID) displayID = new int( 0 );

	// Allow colors outside the normal range [0..1]
	glClampColorARB( GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE );
    glClampColorARB( GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE );
    glClampColorARB( GL_CLAMP_READ_COLOR_ARB, GL_FALSE );

	// Run the display!
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
	glEnable( GL_NORMALIZE );
	glEnable( GL_CULL_FACE );

	// Enable all the lights needed
	for (int i=0; i < *data->ui->numLightsUsed; i++)
	{
		GLLight *lght = scene->GetLight(i);
		if (lght) lght->Enable();
	}

	// This is a bit of a nonsense call, we don't really need
	//   most of what this function does.  However, it DOES
	//   update the state of the internal scene variable "numLightsEnabled"
	//   which, somehow, appears out-of-date on some non-debug 
	//   (i.e., Release) builds, causing the program to crash
	//   immediately upon entering the glutMainLoop().
	// I really need to just fix this by changing the abstraction
	//   for how lights are enabled above (and elsewhere).  But
	//   when the bug shows up just as the demo is almost out 
	//   the door...
	scene->SetupEnabledLightsWithCurrentModelview();

	// Start rendering!
	glutMainLoop();

	// Should never be hit, but compilers seem to need it...
	return 0;
}
