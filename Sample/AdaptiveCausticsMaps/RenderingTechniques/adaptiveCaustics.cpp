/***************************************************************************/
/* adaptiveCaustics.cpp                                                    */
/* -----------------------                                                 */
/*                                                                         */
/* Draw refractive caustics using an adaptive rendering of the refractive  */
/*    geometry.  This helps avoid costly full-screen rendering, and allows */
/*    photons to actually be generated at different resolutions (instead   */
/*    of generating them at full resolution and then grouping them).  This */
/*    opens up possibilities for more intelligent photon generation, for   */
/*    instance only generating important photons at higher resolution.     */
/*                                                                         */
/* Chris Wyman (05/02/2008)                                                */
/***************************************************************************/


#include "causticDemo.h"
#include "renderingData.h"
#include "renderingMethods.h"

extern Scene *scene;
extern RenderingData *data;

void CreateBackgroundTex( FrameBuffer *fb, bool withCaustics );
void CreateRefractionBufs( unsigned int drawWhat );

// This function takes in an input light and computes two intermediate
//    textures.  The first (in the FBO caustic_BackgroundOBJs) stores
//    the positions of opaque geometry visible in the light frustum.
//    The second (in the FBO deferredCaustic_refractorGeom) stores the
//    refractor's front and back side surface normals.
void CreateBuffersForAdaptiveCaustics( int lightNum )
{
	// Identify the background objects, from lightNum's point of view
	data->fbo->caustic_BackgroundOBJs->BindBuffer();
		glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
		glClear( GL_DEPTH_BUFFER_BIT );
		glLoadIdentity();
		scene->LightLookAtMatrix( lightNum );
		data->shader->outputEyeSpacePosition->EnableShader();
		scene->DrawOnly( OBJECT_FLAGS_ISBACKGROUND, MATL_FLAGS_NONE,
						 OBJECT_OPTION_USE_LOWRES, true );
		data->shader->outputEyeSpacePosition->DisableShader();
		glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	data->fbo->caustic_BackgroundOBJs->UnbindBuffer();

	// Compute both front and back facing position and normal maps.
	data->fbo->deferredCaustic_refractorGeom->BindBuffer();
		glClearColor(0,0,0,1);
		data->fbo->deferredCaustic_refractorGeom->ClearBuffers();
		glLoadIdentity();
		scene->LightLookAtMatrix( lightNum );
		glDisable( GL_CULL_FACE );
		data->shader->deferRefract_renderFrontAndBackNorms->EnableShader();
		scene->DrawOnly( OBJECT_FLAGS_ISREFRACTIVE, MATL_FLAGS_NONE, OBJECT_OPTION_NONE, true );
		data->shader->deferRefract_renderFrontAndBackNorms->DisableShader();
		glEnable( GL_CULL_FACE );
	data->fbo->deferredCaustic_refractorGeom->UnbindBuffer();
	data->fbo->deferredCaustic_refractorGeom->AutomaticallyGenerateMipmaps( FBO_COLOR0 );
}

// Actually draw the scene with adaptively sampled photons
void Display_WithAdaptiveCaustics( void )
{
	float glassColor[4] = {1.0, 1.0, 1.0, 0.0};

	// Generate shadow map(s)...
	for (int i=0; i < scene->NumLightsEnabled(); i++)
		scene->CreateShadowMap( data->fbo->shadowMap[i], 
								data->param->shadowMatrixTrans[i], 
								i, // Light Number
								*(data->param->shadowMapBias) );

	for (int lightNum=0; lightNum < scene->NumLightsEnabled(); lightNum++)
	{
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		scene->LightPerspectiveMatrix( lightNum, 1 );
		glMatrixMode( GL_MODELVIEW );

		// Create the temporaries (i.e., the geometry buffers) for the deferred
		//    refraction pass seen from the light
		CreateBuffersForAdaptiveCaustics( lightNum );

		// OK.  Now that we have all the information to compute all the photons
		//    stored in our temporary FBOs, adaptively traverse the "mipmap" levels
		//    of our virtual photon buffer.  In other words, start at a 64x64
		//    resolution, compute all the photons, and then in each area where 
		//    the photons hit the refractor, adaptively subdivide (don't subdivide
		//    in other regions).
		data->fbo->causticMap[lightNum]->BindBuffer();
		data->fbo->causticMap[lightNum]->ClearBuffers();
		glLoadIdentity();
		scene->LightLookAtMatrix( lightNum );

		// The actual hierarchical traversal is given in code in utilRoutines.cpp.  This
		//    code traverses our photon hierarchy and outputs the correct photons as 
		//    splats into the current buffer (the causticMap[lightNum] enabled above)
		glBlendFunc( GL_ONE, GL_ONE );
		int startLevel = 6;  // The starting traversal level (2^6 = 64x64 photons)
		
		HierarchicalTransformFeedbackTraversalWithBatching( data->shader->adaptCaustic_traverse, 
			                                    data->shader->adaptCaustic_splat,
												startLevel, 
												lightNum );

		// All right, we just created the caustic map, so do some housekeeping to make sure
		//    our rendering pipeline (see directories "Scene," "Objects" and "Materials") can 
		//    correctly render geometry using a caustic map.
		data->fbo->causticMap[lightNum]->UnbindBuffer();

		// We only want the caustic map projected onto the diffuse objects closest to the light,
		//    so we need to remember the depth of those surfaces.  We computed this already in
		//    the CreateBuffersForAdaptiveCaustics() function, so we just copy it into the
		//    caustic map FBO so it is in an obvious location.
		CopyDepthToCausticMap( data->fbo->causticMap[lightNum], data->fbo->caustic_BackgroundOBJs );

		// Setup the scene renderer so it knows where this caustic map is stored.
		scene->SetCausticMapID( lightNum, data->fbo->causticMap[lightNum]->GetColorTextureID() );
		scene->SetCausticDepthID( lightNum, data->fbo->causticMap[lightNum]->GetDepthTextureID() );

		glMatrixMode( GL_PROJECTION );
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
	}

	// OK.  Now we've got our shadow maps and our caustic maps, so we need
	//     to actually draw the scene from the eye.  Below is this code.

	// First, render the buffers needed for deferred refraction from the 
	//     eye's point of view
	CreateBackgroundTex( data->fbo->refract_BackgroundOBJs, true );
	CreateRefractionBufs( OBJECT_FLAGS_ISREFRACTIVE );

	// Now we're ready to begin actually drawing the final image from the eye's point of view
	data->fbo->mainWin->BindBuffer();
	glLoadIdentity();
	scene->LookAtMatrix();
	scene->SetupEnabledLightsWithCurrentModelview();

	// First draw all the opaque, non refractive geometry.
	//    However, we already drew this in CreateBackgroundTex(), so instead of
	//    repeating this (possibly expensive) draw routine, we'll just copy the
	//    image and depth of the scene we obtained previously.
	glDisable( GL_DEPTH_TEST );
	CopyColorAndDepthTexturesIntoCurrentFBO( data->fbo->refract_BackgroundOBJs->GetColorTextureID(), 
											 data->fbo->refract_BackgroundOBJs->GetDepthTextureID() );
	glEnable( GL_DEPTH_TEST );

	// Now that we have the non-refractive stuff draw, draw our glass object.
	DeferredRefraction( data->fbo->refract_BackgroundOBJs, glassColor );

	// That's it.  We're done drawing this frame.
	data->fbo->mainWin->UnbindBuffer();
}



