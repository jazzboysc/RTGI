/***************************************************************************/
/* initializeRenderingData.cpp                                             */
/* -----------------------                                                 */
/*                                                                         */
/* Initialization routines for data needed for all the rendering methods   */
/*    in this directory.                                                   */
/*                                                                         */
/* Chris Wyman (02/23/2008)                                                */
/***************************************************************************/


#include "causticDemo.h"
#include "renderingData.h"
#include "renderingMethods.h"

extern Scene *scene;
RenderingData *data = 0;

void InitializeMinCausticHierarchy( GLuint causticBufID, int resolution );

// This initializes the RenderingData structure declared above, which is my way
//    of using global data without a lot of the problems that typically
//    are associated with it (i.e., lists of variables at the top of each
//    file, poor organization, etc., etc.).
// For data or data structures not directly associated with the particular
//    scene we have loaded, this is where they are stored.
// In particular, below are the framebuffer objects, vertex buffer objects,
//    non-scene specific textures, shaders and parameters specific to
//    our experimental shaders, and program-specific UI data.
void InitializeRenderingData( void )
{
	// Check to make sure we only initialize once.
	if (data) return;

	// Allocate global data structures
	data = new RenderingData();
	data->fbo    = new FrameBufferData();
	data->param  = new ParameterData();
	data->shader = new ShaderData();
	data->tex    = new TextureData();
	data->vbo    = new VertexBufferData();
	data->glID   = new OtherGLIDData();
	data->ui     = new UIData();

	// Now declare our framebuffer objects that various rendering modes will draw too

	// Framebuffer to render as the main window (later downsampled for antialiasing).
	//    Probably we don't need this, but I've found it convenient to use have the
	//    "main window" be a FBO for a large number of reasons.  However, it does
	//    definitely slow the the program down....  For speed, draw to the real window.
	data->fbo->mainWin = new FrameBuffer( GL_TEXTURE_2D, 2*scene->GetWidth(), 2*scene->GetHeight(), -1, 
										  GL_RGBA, 1, 1, false, "Main Render Window" );
	data->fbo->mainWin->CheckFramebufferStatus( 1 );

	// Framebuffer to render help instructions.  This probably isn't needed either,
	//    but in my experience GLUT draws text exceedingly slowly, and I'm trading
	//    space (for this FBO) to reduce per-frame text rendering costs when displaying
	//    the help screen.  For speed, use a really good UI/text rendering package.
	data->fbo->helpScreen = new FrameBuffer( GL_TEXTURE_2D, scene->GetWidth(), scene->GetHeight(), -1, 
										  GL_RGBA, 1, 0, false, "Help Screen" );
	data->fbo->helpScreen->CheckFramebufferStatus( 1 );

	// Framebuffer to store the shadow map(s).  These are pretty big, perhaps bigger
	//    than they need to be, but I see no reason to have poor quality shadows
	//    distracting from the evaluation of our caustics.
	if (scene->GetNumLights() > 1)
		printf("    (-) Allocating buffers for %d light sources...\n", scene->GetNumLights());
	for (int i=0; i < scene->GetNumLights(); i++)
	{
		data->fbo->shadowMap[i] = new FrameBuffer( GL_TEXTURE_2D, 2*scene->GetWidth(), 2*scene->GetHeight(), -1, 
												GL_ALPHA, 1, 1, false, "Shadow Map" );
		data->fbo->shadowMap[i]->CheckFramebufferStatus( 1 );
	}

	// FBOs to store background images (for geometry seen behind the refractor).
	//    There's two here, one for rendering refractive stuff, one for rendering
	//    the caustic backgrounds.  They can probably be reused, if the code is
	//    structured carefully.  But in our prototype, we want to be able to 
	//    independently change their resolutions as we tweak the parameters.  
	data->fbo->refract_BackgroundOBJs = new FrameBuffer( GL_TEXTURE_2D, scene->GetWidth(), scene->GetHeight(), 
														 -1, GL_RGBA, 1, 1, false, "Refractor Background Objects" );
	data->fbo->refract_BackgroundOBJs->CheckFramebufferStatus( 1 );
	data->fbo->caustic_BackgroundOBJs = new FrameBuffer( GL_TEXTURE_2D, scene->GetWidth(), scene->GetHeight(), 
														 -1, GL_RGBA16F_ARB, 1, 1, false, "Caustic Background Objects" );
	data->fbo->caustic_BackgroundOBJs->CheckFramebufferStatus( 1 );

	// FBO to store photon buffer (only needed for the "originalCausticMap" and 
	//    "originalHierarchicalCausticMap" rendering modes).  This is a big buffer
	//    that is expensive to create and process, which is the whole goal behind
	//    the adaptive approach described in our EG paper.
	data->fbo->caustic_PhotonBuffer = new FrameBuffer( GL_TEXTURE_2D, 1024, 1024, 
													  -1, GL_RGBA16F_ARB, 1, 1, true, "Caustic Photon Buffer" );
	data->fbo->caustic_PhotonBuffer->CheckFramebufferStatus( 1 );


	// Below are FBOs to store temporary refraction buffers

	// This 1-layer FBO is only used for non-deferred refraction.
	data->fbo->refract_NormalMap = new FrameBuffer( GL_TEXTURE_2D, scene->GetWidth(), scene->GetHeight(), 
													-1, GL_RGBA, 1, 1, false, "Refractor Normal Map" );
	data->fbo->refract_NormalMap->CheckFramebufferStatus( 1 );

	// This 2-layer FBO is used for deferred refraction.  
	data->fbo->deferredRefract_refractorGeom = new FrameBuffer( GL_TEXTURE_2D_ARRAY_EXT, scene->GetWidth(), scene->GetHeight(), 2,
														GL_RGBA16F_ARB, 1, 1, true, "Adaptive Refraction Test" );
	data->fbo->deferredRefract_refractorGeom->CheckFramebufferStatus( 1 );

	// This 2-layer FBo is used for deferred caustic normal geometry.
	data->fbo->deferredCaustic_refractorGeom = new FrameBuffer( GL_TEXTURE_2D_ARRAY_EXT, 2*1024, 2*1024, 2,
														GL_RGBA16F_ARB, 1, 1, true, "Adaptive Refraction Test" );
	data->fbo->deferredCaustic_refractorGeom->CheckFramebufferStatus( 1 );


	// Allocate FBOs to store the final caustic maps for each of the enabled lights
	for (int i=0; i < scene->GetNumLights(); i++)
	{
		data->fbo->causticMap[i] = new FrameBuffer( GL_TEXTURE_2D, 1024, 1024, 
												 -1, GL_RGBA16F_ARB, 1, 1, false, "Caustic Map" );
		data->fbo->causticMap[i]->CheckFramebufferStatus( 1 );
	}

	// This "small" caustic map is used for the adaptive hierarchical caustic maps.  
	//   I *should* use the layered one allocated below, as done in the standard 
	//   hierarchical caustic maps.
	data->fbo->causticMapSmall = new FrameBuffer( GL_TEXTURE_2D, 512, 512, 
											 -1, GL_RGBA16F_ARB, 1, 0, false, "Caustic Map" );
	data->fbo->causticMapSmall->CheckFramebufferStatus( 1 );

	// This multi-layer caustic map is only partially used, I use this target as a "render-to-mipmap",
	//    which means I should only need 1 1/3 the memory of a standard FBO, not 4x the memory!  
	//    Currently, this is only used in the implementation of the Wyman 2008 "Hierarchical Caustic
	//    Map" paper, and I use a 2-layer (instead of 4-layer) approximation for the adaptive version.
	data->fbo->causticMapLayered = new FrameBuffer( GL_TEXTURE_2D_ARRAY_EXT, 1024, 1024, 4,
		                                            GL_RGBA16F_ARB, 1, 0, false, "Layered Caustic Map" );
	data->fbo->causticMapLayered->CheckFramebufferStatus( 1 );

	// Initialize and/or allocate various textures not specific to the scene.
	//    Technically, the spotlight is actually a scene texture, but to avoid
	//    possibility of the scene forgetting to define it, below we check to
	//    make sure the scene defined these textures, and if not we load a default.
	data->tex->spotlight = scene->GetNamedTexture( "spotlight" );
	if (!data->tex->spotlight)
	{
		char *file = scene->paths->GetTexturePath( "spot_white.ppm" );
		data->tex->spotlight = new GLTexture( file, 
		                                      TEXTURE_REPEAT_S | TEXTURE_REPEAT_T | TEXTURE_MIN_LINEAR_MIP_LINEAR );
		free( file );
	}
	data->tex->lowResSpot = scene->GetNamedTexture( "spotlightlowres" );
	if (!data->tex->lowResSpot)
	{
		char *file = scene->paths->GetTexturePath( "spot_white64.ppm" );
		data->tex->lowResSpot = new GLTexture( file, 
		                                      TEXTURE_REPEAT_S | TEXTURE_REPEAT_T | TEXTURE_MIN_LINEAR_MIP_LINEAR );
		free( file );
	}

	// Load the Iowa logo (used on the help screen)
	{
		char *file = scene->paths->GetTexturePath( "iowaLogo.ppm" );
		data->tex->iowaLogo = new GLTexture( file, 
		                                      TEXTURE_REPEAT_S | TEXTURE_REPEAT_T | TEXTURE_MIN_LINEAR_MIP_LINEAR );
		free( file );
	}

	// Save a simple glOrtho projection matrix, which we use in a couple
	//    of our shaders.
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,1,0,1);
	glGetFloatv( GL_MODELVIEW_MATRIX, data->param->gluOrtho );
	glPopMatrix();


	// Initialize starting UI information
	data->ui->updateHelpScreen = true;
	data->ui->displayHelp = true;
	data->ui->captureScreen = false;


	// Setup the "scene variables."  Scene variables are defined in the 
	//    scenefile.  This is actually a bit of a clumsy way of doing it.  
	//    Anyways, if the scene file fails to declare variables needed 
	//    by our rendering modes, we need to assign some default values to them.
	data->param->shadowMapBias = scene->GetSceneFloatVar( "shadowmapbias" );
	if (!data->param->shadowMapBias)
		data->param->shadowMapBias = new float( -0.005 );
	data->param->indexOfRefraction = scene->GetSceneFloatVar( "idxrefract" );
	if (!data->param->indexOfRefraction)
		data->param->indexOfRefraction = new float( 1.1 );
	data->param->maxTraversalLevel = scene->GetSceneIntVar( "maxtraversallevel" );
	if (!data->param->maxTraversalLevel)
		data->param->maxTraversalLevel = new int( 10 );
	data->param->lightFOV = scene->GetSceneFloatVar( "lightfov" );
	data->ui->animation = scene->GetSceneBoolVar( "animation" );
	if (!data->ui->animation)
		data->ui->animation = new bool( true );
	data->ui->numBatches = scene->GetSceneIntVar( "numbatches" );
	if (!data->ui->numBatches)
		data->ui->numBatches = new int( 2 );
	data->ui->numLightsUsed = scene->GetSceneIntVar( "numlightsused" );
	if (!data->ui->numLightsUsed)
		data->ui->numLightsUsed = new int( 1 );
	data->ui->adaptiveDepthParam = scene->GetSceneIntVar( "adaptivedepthparam" );
	if (!data->ui->adaptiveDepthParam)
		data->ui->adaptiveDepthParam = new int( 5 );


	
	// Allocate, load, and initialize all the non-scene specific shaders.  This
	//    includes all the shaders needed for various utility functions as well
	//    as all the shaders needed for our refraction and caustic rendering code.
	// In some final application, these might actually be part of the "scene," but
	//    for now they're part of our prototype rendering modes and are not 
	//    considered a part of the scene.

	// A shader to copy a depth buffer from one FBO to another
	scene->AddShader( data->shader->copyDepth = 
					  new GLSLProgram( NULL, 
									   NULL, 
									   "copyDepth.frag.glsl", 
									   true, scene->paths->GetShaderPathList() ) );

	// The two shaders needed for the two passes of refraction from
	//    the image-space refraction (SIGGRAPH '05) paper.
	scene->AddShader( data->shader->refract_DrawNormals = 
					  new GLSLProgram( "renderJustBackNormals.Vert.glsl", 
									   NULL, 
									   "renderJustBackNormals.Frag.glsl", 
									   true, scene->paths->GetShaderPathList() ) );
	scene->AddShader( data->shader->refract = 
					  new GLSLProgram( "refractionWithBackgroundTexture.Vert.glsl", 
									   NULL, 
									   "refractionWithBackgroundTexture.Frag.glsl", 
									   true, scene->paths->GetShaderPathList() ) );

	// The two shaders needed for basic image-space splatting of caustics (I3D '2006)
	scene->AddShader( data->shader->caustic_DrawPhotonBuf = 
					  new GLSLProgram( "createPhotonBuffer.vert.glsl", 
									   NULL, 
									   "createPhotonBuffer.frag.glsl", 
									   true, scene->paths->GetShaderPathList() ) );
	scene->AddShader( data->shader->caustic_SimplePhotonSplat = 
					  new GLSLProgram( "simplePhotonSplat.vert.glsl", 
								       "simplePhotonSplat.geom.glsl", 
									   "simplePhotonSplat.frag.glsl", 
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->caustic_SimplePhotonSplat->GeometryShaderSettings( GL_POINTS, 1, GL_POINTS );
	data->shader->caustic_SimplePhotonSplat->LinkProgram();

	// The two shaders needed for hierarchical caustic mapping (I3D '2008)
	scene->AddShader( data->shader->caustic_TraversePhotonHier = 
					  new GLSLProgram( "traversePhotonHierarchy.vert.glsl",
					                   "traversePhotonHierarchy.geom.glsl",
									   "traversePhotonHierarchy.frag.glsl",
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->caustic_TraversePhotonHier->GeometryShaderSettings( GL_POINTS, 4, GL_POINTS );
	data->shader->caustic_TraversePhotonHier->LinkProgram();
	scene->AddShader( data->shader->caustic_SplatPhotonHier = 
					  new GLSLProgram( "finalHierarchySplat.vert.glsl",
					                   "finalHierarchySplat.geom.glsl",
									   "finalHierarchySplat.frag.glsl",
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->caustic_SplatPhotonHier->GeometryShaderSettings( GL_POINTS, 4, GL_POINTS );
	data->shader->caustic_SplatPhotonHier->LinkProgram();

	// A shader to copy both a FBO's color and depth buffer into another FBO
	scene->AddShader( data->shader->copyColorAndDepth = 
					  new GLSLProgram( NULL, 
									   NULL, 
									   "copyColorAndDepth.frag.glsl", 
									   true, scene->paths->GetShaderPathList() ) );

	// A shader to combine layers in our hierarchical caustic map's "mipmap" layers
	//     (actually an texture array) into a single layer final caustic map
	scene->AddShader( data->shader->combineMipmapArrayLevels =
                      new GLSLProgram( NULL, 
									   NULL, 
									   "mergeRenderToMipmapArrayLevels.frag.glsl", 
									   true, scene->paths->GetShaderPathList() ) );

	// Instead of save the color when rendering a scene, this stores fragment 
	//     position (x,y,z) as (r,g,b)
	scene->AddShader( data->shader->outputEyeSpacePosition =
					  new GLSLProgram( "outputFragmentEyePosition.vert.glsl",
					                   NULL,
									   "outputFragmentEyePosition.frag.glsl",
									   true, scene->paths->GetShaderPathList() ) );

	// Two shaders require to perform the deferred refraction (in this EG paper)
	//     note the actual refraction shader has two version, one that only handles
	//     one light, on that handles two (which shows up in the Phong highlights
	//     that stand in for accurate reflections due to Fresnel effects).
	scene->AddShader( data->shader->deferredRefraction = 
		              new GLSLProgram( "deferredRefraction.vert.glsl",
					                   NULL,
									   (scene->GetNumLights() > 1 ? "multiLightDeferredRefraction.frag.glsl" : "deferredRefraction.frag.glsl"),
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->deferredRefraction->SetProgramDisables( GLSL_BLEND | GLSL_DEPTH_TEST );
	scene->AddShader( data->shader->deferRefract_renderFrontAndBackNorms = 
					  new GLSLProgram( "renderFrontAndBackNormals.vert.glsl",
					                   "renderFrontAndBackNormals.geom.glsl",
									   "renderFrontAndBackNormals.frag.glsl",
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->deferRefract_renderFrontAndBackNorms->GeometryShaderSettings( GL_TRIANGLES, 3, GL_TRIANGLE_STRIP );
	data->shader->deferRefract_renderFrontAndBackNorms->LinkProgram();


	// Two shaders used by the adaptive creation of photons described by the EG paper.
	//     These two shaders use the maximal traversal metric for halting traversal,
	//     and are used in adaptiveCaustics.cpp (rendering mode "4" in the demo).
	scene->AddShader( data->shader->adaptCaustic_traverse =
		              new GLSLProgram( "adaptiveCaustics_PassDownVertexPosition.vert.glsl",
					                   "adaptiveCaustics_maxTraversal_TraversalShader.geom.glsl",
									   "adaptiveCaustics_TraversalShader.frag.glsl",
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->adaptCaustic_traverse->GeometryShaderSettings( GL_POINTS, 4, GL_POINTS );
	data->shader->adaptCaustic_traverse->LinkProgram();
	scene->AddShader( data->shader->adaptCaustic_splat =
		              new GLSLProgram( "adaptiveCaustics_SplatIntoCMap.vert.glsl",
					                   NULL,
									   "adaptiveCaustics_maxTraversal_SplatIntoCMap.frag.glsl",
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->adaptCaustic_splat->SetupAutomaticTextureBinding( "geomNorm", data->fbo->deferredCaustic_refractorGeom->GetColorTextureID( 0 ), GL_TEXTURE1, GL_TEXTURE_2D_ARRAY_EXT );
	data->shader->adaptCaustic_splat->SetupAutomaticTextureBinding( "geomDepth", data->fbo->deferredCaustic_refractorGeom->GetDepthTextureID(), GL_TEXTURE2, GL_TEXTURE_2D_ARRAY_EXT );
	data->shader->adaptCaustic_splat->SetupAutomaticTextureBinding( "otherObjsDepth", data->fbo->caustic_BackgroundOBJs->GetDepthTextureID(), GL_TEXTURE4, GL_TEXTURE_2D );
	data->shader->adaptCaustic_splat->SetupAutomaticTextureBinding( "spotLight", data->tex->spotlight->TextureID(), GL_TEXTURE6, GL_TEXTURE_2D );
	data->shader->adaptCaustic_splat->SetProgramEnables( GLSL_VARY_POINT_SIZE | GLSL_BLEND );
	data->shader->adaptCaustic_splat->SetProgramDisables( GLSL_DEPTH_TEST );



	// First step in 2nd traversal for adaptive w/error metric (find photon hits, detect convergence)
	scene->AddShader( data->shader->adaptiveCaustics_errorMetric_detectConvergence =
		              new GLSLProgram( "adaptiveCaustics_SplatIntoCMap.vert.glsl",
					                   "adaptiveCaustics_detectConvergingPhotons.geom.glsl",
									   NULL,
									   false, scene->paths->GetShaderPathList() ) );
	data->shader->adaptiveCaustics_errorMetric_detectConvergence->SetupAutomaticTextureBinding( "geomNorm", data->fbo->deferredCaustic_refractorGeom->GetColorTextureID( 0 ), GL_TEXTURE1, GL_TEXTURE_2D_ARRAY_EXT );
	data->shader->adaptiveCaustics_errorMetric_detectConvergence->SetupAutomaticTextureBinding( "geomDepth", data->fbo->deferredCaustic_refractorGeom->GetDepthTextureID(), GL_TEXTURE2, GL_TEXTURE_2D_ARRAY_EXT );
	data->shader->adaptiveCaustics_errorMetric_detectConvergence->SetupAutomaticTextureBinding( "otherObjsDepth", data->fbo->caustic_BackgroundOBJs->GetDepthTextureID(), GL_TEXTURE4, GL_TEXTURE_2D );
	data->shader->adaptiveCaustics_errorMetric_detectConvergence->SetupAutomaticTextureBinding( "spotLight", data->tex->spotlight->TextureID(), GL_TEXTURE6, GL_TEXTURE_2D );
	data->shader->adaptiveCaustics_errorMetric_detectConvergence->GeometryShaderSettings( GL_LINES_ADJACENCY_EXT, 4, GL_POINTS );
	data->shader->adaptiveCaustics_errorMetric_detectConvergence->ReloadShaders();

	// Second step in 2nd traversl for adaptive w/error metric (concat converged photons to final photon list)
	scene->AddShader( data->shader->adaptiveCaustics_errorMetric_streamOutConverged =
		              new GLSLProgram( "adaptiveCaustics_PassDownVertexPosition.vert.glsl",
					                   "adaptiveCaustics_streamOutConvergedPhotons.geom.glsl",
									   NULL,
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->adaptiveCaustics_errorMetric_streamOutConverged->GeometryShaderSettings( GL_POINTS, 1, GL_POINTS );
	data->shader->adaptiveCaustics_errorMetric_streamOutConverged->ReloadShaders();

	// Third step in 2nd traversal in adaptive w/error metric  (split current photon into 4 children)
	scene->AddShader( data->shader->adaptiveCaustics_errorMetric_Traversal =
		              new GLSLProgram( "adaptiveCaustics_PassDownVertexPosition.vert.glsl",
					                   "adaptiveCaustics_errorMetric_TraversalShader.geom.glsl",
									   "adaptiveCaustics_TraversalShader.frag.glsl",
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->adaptiveCaustics_errorMetric_Traversal->GeometryShaderSettings( GL_POINTS, 4, GL_POINTS );
	data->shader->adaptiveCaustics_errorMetric_Traversal->LinkProgram();
	
	// Final step in adaptive w/error metric (output uncoverged photons in splattable format)
	scene->AddShader( data->shader->adaptiveCaustics_errorMetric_streamOutUnconverged =
		              new GLSLProgram( "adaptiveCaustics_PassDownVertexPosition.vert.glsl",
					                   "adaptiveCaustics_streamOutUnConvergedPhotons.geom.glsl",
									   NULL,
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->adaptiveCaustics_errorMetric_streamOutUnconverged->GeometryShaderSettings( GL_POINTS, 1, GL_POINTS );
	data->shader->adaptiveCaustics_errorMetric_streamOutUnconverged->ReloadShaders();

	//  A shader to actually splat photons computed using the adaptive traversal w/error metric
	scene->AddShader( data->shader->adaptiveCaustics_errorMetric_SplatPhotons =
	                  new GLSLProgram( "adaptiveCaustics_alreadyComputedPhoton.vert.glsl",
					                   NULL,
									   "adaptiveCaustics_splatPreviouslyComputedPhotons.frag.glsl",
									   true, scene->paths->GetShaderPathList() ) );
	data->shader->adaptiveCaustics_errorMetric_SplatPhotons->SetProgramEnables( GLSL_BLEND );
	data->shader->adaptiveCaustics_errorMetric_SplatPhotons->SetProgramDisables( GLSL_DEPTH_TEST );


	// Initialize various vertex buffer objects.

	// First a low-resolution start buffer to begin our hierarchical traversal
	//    (so we need not start from a single photon).
	glGenBuffers( 1, &data->vbo->genericTraversalStartBuffer );
	InitializeMinCausticHierarchy( data->vbo->genericTraversalStartBuffer, 64 );


	// Now we need more generic, large traversal buffers to ping-pong between
	//     for other rendering modes.  Here I allocate an array of these.  Many
	//     render modes need only 1 or 2, those some of the complex ones need
	//     4 or 5.  They probably need not all be the same size, but if I use
	//     different sizes, I have to remember which is which!  (And then they're
	//     not generic!).
	GLsizeiptr traversalBufSz = 2048 * 2048 * sizeof(float) * 4;
	glGenBuffers( 5, data->vbo->genericTraversalBuffer );
	glBindBuffer( GL_ARRAY_BUFFER, data->vbo->genericTraversalBuffer[0] );
	glBufferData( GL_ARRAY_BUFFER, traversalBufSz, NULL, GL_STREAM_COPY );   
	glBindBuffer( GL_ARRAY_BUFFER, data->vbo->genericTraversalBuffer[1] );
	glBufferData( GL_ARRAY_BUFFER, traversalBufSz, NULL, GL_STREAM_COPY );   
	glBindBuffer( GL_ARRAY_BUFFER, data->vbo->genericTraversalBuffer[2] );
	glBufferData( GL_ARRAY_BUFFER, traversalBufSz, NULL, GL_STREAM_COPY );   
	glBindBuffer( GL_ARRAY_BUFFER, data->vbo->genericTraversalBuffer[3] );
	glBufferData( GL_ARRAY_BUFFER, traversalBufSz, NULL, GL_STREAM_COPY );   
	glBindBuffer( GL_ARRAY_BUFFER, data->vbo->genericTraversalBuffer[4] );
	glBufferData( GL_ARRAY_BUFFER, traversalBufSz, NULL, GL_STREAM_COPY );   
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	// Initialize various other OpenGL identifier data
	glGenQueries( 1, &data->glID->primCountQuery  );


	if (scene->IsVerbose())
		printf("    (-) Finished InitializeRenderingData()...\n");
}



void InitializeMinCausticHierarchy( GLuint causticBufID, int resolution )
{
	float *causticStartPoints = (float *)malloc( resolution * resolution * 4 * sizeof( float ) );
	for (int i = 0; i < resolution * resolution; i++)
	{
		int x = i % resolution;
		int y = i / resolution;
		causticStartPoints[4*i+0] = x/(float)resolution;
		causticStartPoints[4*i+1] = y/(float)resolution;
		causticStartPoints[4*i+2] = causticStartPoints[4*i+3] = 0;
	}
	glBindBuffer( GL_ARRAY_BUFFER, causticBufID );
	glBufferData( GL_ARRAY_BUFFER, resolution*resolution*4*sizeof( float ), causticStartPoints, GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	free( causticStartPoints );
}


void FreeMemoryAndQuit( void )
{
	// Print a status message
	printf("(+) Exiting program....\n");
	printf("    (-) Freeing resources...\n");

	// Free FBOs
	delete data->fbo->mainWin;
	delete data->fbo->deferredCaustic_refractorGeom;
	delete data->fbo->deferredRefract_refractorGeom;
	delete data->fbo->refract_BackgroundOBJs;
	delete data->fbo->caustic_BackgroundOBJs;
	for (int i=0; i < scene->GetNumLights(); i++)
	{
		delete data->fbo->shadowMap[i];
		delete data->fbo->causticMap[i];
	}

	// Free VBOs
	glDeleteBuffers( 5, data->vbo->genericTraversalBuffer );

	// Free the scene (which frees assorted associated data)
	delete scene;

	// Close the GLUT window.
	glutDestroyWindow( glutGetWindow() );

	// Done cleaning up!  Quit!
	printf("    (-) Done.\n");
	exit(0);
}

