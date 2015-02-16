/***************************************************************************/
/* adaptiveCausticsErrorMetric.cpp                                         */
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

// Refraction & caustics code from other rendering modes
void CreateBackgroundTex( FrameBuffer *fb, bool withCaustics );
void CreateRefractionBufs( unsigned int drawWhat );
void DisplayTexture( GLuint texID );
void CreateBuffersForAdaptiveCaustics( int lightNum );

// Takes a list of photons that need to be splatted to the caustic map, 
//    and performs the splatting.
void RenderSplats( GLuint photonBuf, int numPhotons, FrameBuffer *causticMap )
{
	GLuint spotLightID = ( data->tex->lowResSpot ? 
		                       data->tex->lowResSpot->TextureID() : 
	                           data->tex->spotlight->TextureID() );
	glPushMatrix();
	glLoadIdentity();
	SetCurrentVertexBuffer( photonBuf );    
	data->shader->adaptiveCaustics_errorMetric_SplatPhotons->EnableShader();
	int maxPhotonRes = (int)(pow(2.0,*data->param->maxTraversalLevel)+0.1);
	data->shader->adaptiveCaustics_errorMetric_SplatPhotons->SetParameter( "splatResolutionModifier", 
		                                                          sqr( causticMap->GetWidth()/(float)maxPhotonRes ) ); 
	data->shader->adaptiveCaustics_errorMetric_SplatPhotons->SetParameter( "renderBufRes", 
		                                                          causticMap->GetWidth() );
	data->shader->adaptiveCaustics_errorMetric_SplatPhotons->BindAndEnableTexture( "spotTex", spotLightID, 
		                                                                  GL_TEXTURE1, GL_TEXTURE_2D );
		glPointSize(3.0);
		glDrawArrays( GL_POINTS, 0, numPhotons );
	data->shader->adaptiveCaustics_errorMetric_SplatPhotons->DisableTexture( GL_TEXTURE1, GL_TEXTURE_2D );
	data->shader->adaptiveCaustics_errorMetric_SplatPhotons->DisableShader();
	glPopMatrix();
}

// The first step of our modified traversal is to compute the photon hit points for
//   every photon and check if photons in a group of 4 (i.e., a GL_LINES_ADJACENCY_EXT)
//   converge to a single texel in the caustic map.  Please note, these 4 pixels 
//   are arranged as when creating a mipmap (i.e., independent 2x2 photon clusters 
//   at each level).
void IdentifyConvergingPhotons( GLuint inputBuffer, GLuint outputBuffer, int primCount, int lightNum, GLuint inputOffsetBytes )
{
	int recordLocs;
	SetCurrentVertexBuffer( inputBuffer, inputOffsetBytes ); 
	data->shader->adaptiveCaustics_errorMetric_detectConvergence->EnableShader();
		recordLocs = glGetVaryingLocationNV( data->shader->adaptiveCaustics_errorMetric_detectConvergence->GetProgramID(), "gl_Position" );
		glTransformFeedbackVaryingsNV( data->shader->adaptiveCaustics_errorMetric_detectConvergence->GetProgramID(), 1, &recordLocs, GL_INTERLEAVED_ATTRIBS_NV );
		glBindBufferBaseNV( GL_TRANSFORM_FEEDBACK_BUFFER_NV, 0, outputBuffer );
		glBeginTransformFeedbackNV( GL_POINTS );
			int maxPhotonRes = (int)(pow(2.0,*data->param->maxTraversalLevel)+0.1);
			float idx = *data->param->indexOfRefraction;
			float _near = scene->GetLight( lightNum )->GetLightNearPlane(), _far = scene->GetLight( lightNum )->GetLightFarPlane();
			data->shader->adaptiveCaustics_errorMetric_detectConvergence->SetParameter( "local1", _near*_far, _far-_near, _far+_near, _far); 
			data->shader->adaptiveCaustics_errorMetric_detectConvergence->SetParameter( "local2", 1.0/idx, 1.0/(idx*idx), idx, idx*idx );
			data->shader->adaptiveCaustics_errorMetric_detectConvergence->SetParameter( "tanLightFovy2", tan( M_PI * scene->GetLight( lightNum )->GetLightFovy() / 360.0 ) );
			glDrawArrays( GL_LINES_ADJACENCY_EXT, 0, primCount ); 
		glEndTransformFeedbackNV();
	data->shader->adaptiveCaustics_errorMetric_detectConvergence->DisableShader();
}

// This code takes an input stream of photons, and applies a program that outputs some subset
//   of the list to the output buffer.  This routine is used once per traversal step to output 
//   a list of converged photons and once after traversal is complete to output the rest of
//   the photons to an unconverged list.
int OutputPhotonSubList( GLSLProgram *streamProgram, GLuint inputBuffer, GLuint outputBuffer, 
						 int primCount, float intensity, int listOffset )
{
	SetCurrentVertexBuffer( inputBuffer );  
	streamProgram->EnableShader();
	streamProgram->SetParameter( "intensityModifier", intensity );
		int recordLocs = glGetVaryingLocationNV( streamProgram->GetProgramID(), "gl_Position" );
		glTransformFeedbackVaryingsNV( streamProgram->GetProgramID(), 1, &recordLocs, GL_INTERLEAVED_ATTRIBS_NV );
		glBindBufferOffsetNV( GL_TRANSFORM_FEEDBACK_BUFFER_NV, 0, outputBuffer, listOffset );
		BeginPrimitiveCount( GL_POINTS, data->glID->primCountQuery  );
			glDrawArrays( GL_POINTS, 0, primCount ); 
		int newCount = EndPrimitiveCount( data->glID->primCountQuery  );
	streamProgram->DisableShader();
	return newCount;
}

// Apply the traversal render pass to perform a single breadth-first hierarchy traversal.
int TraversalPass( GLSLProgram *traverse, GLuint inputBuffer, GLuint outputBuffer,
				    int deferredBufferWidth, int mipLevel, int inputPrims )
{
	glBindBufferBaseNV( GL_TRANSFORM_FEEDBACK_BUFFER_NV, 0, outputBuffer );
	BeginPrimitiveCount( GL_POINTS, data->glID->primCountQuery  );
	SetCurrentVertexBuffer( inputBuffer );    
		int resolution = deferredBufferWidth / (int)( pow( 2.0f, (float)mipLevel ) + 0.5 );  
		traverse->SetParameter( "deltaHierRes", 0.5f/resolution );    
		traverse->SetParameter( "currHierarchyLevel", (float)mipLevel );
		glDrawArrays( GL_POINTS, 0, inputPrims );  
	return EndPrimitiveCount( data->glID->primCountQuery  );
}

// Apply the modified "traversal" render pass to split unconverged photons into 4 children 
//    at a higher subdivision level.  Please note:  Only unconverged photons ever get
//    passed through this function.
int ModifiedTraversalPass( GLSLProgram *traverse, int deferredBufferWidth, int mipLevel, int inputPrims,
						   GLuint screenPosBuf, GLuint photonHitBuf, GLuint outputBuf, GLuint inputOffsetBytes )
{
	int count = 0, recordLocs;
	// Enable the traversal shader
	traverse->EnableShader();

	// Do some setup for the traversal.  Note this code must come after the traverse shader is enabled!
	recordLocs = glGetVaryingLocationNV( traverse->GetProgramID(), "gl_Position" );
	glTransformFeedbackVaryingsNV( traverse->GetProgramID(), 1, &recordLocs, GL_INTERLEAVED_ATTRIBS_NV );

	// Setup the output buffers
	glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	glBindBufferBaseNV( GL_TRANSFORM_FEEDBACK_BUFFER_NV, 0, outputBuf ); //data->vbo->genericTraversalBuffer[1] );
	BeginPrimitiveCount( GL_POINTS, data->glID->primCountQuery  );

	// Setup the input buffers.  One (the vertex pointer) stores screen space positions
	//   The other (the tex coord pointer) stores photon hit points with the w component
	//   noting if they are converged.
	glBindBuffer( GL_ARRAY_BUFFER, screenPosBuf );    
	glVertexPointer( 4, GL_FLOAT, 0, BUFFER_OFFSET( inputOffsetBytes ) );  
	glBindBuffer( GL_ARRAY_BUFFER, photonHitBuf ); 
	glTexCoordPointer( 4, GL_FLOAT, 0, BUFFER_OFFSET( 0 ) );  
	
		int resolution = deferredBufferWidth;
		if (mipLevel > 0) 
			resolution /= (int)( pow( 2.0f, (float)mipLevel ) + 0.5 ); 
		else 
			resolution *= (int)( pow( 2.0f, (float)-mipLevel ) + 0.5 ); 
		traverse->SetParameter( "deltaHierRes", 0.5f/resolution );    
		glDrawArrays( GL_POINTS, 0, inputPrims ); 

	count = EndPrimitiveCount( data->glID->primCountQuery  );
	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	traverse->DisableShader();
	return count;
}

// This is a modified version of the traversal in utilRoutines.cpp which handles traversal to
//    a maximum specified error.  In this case, the implied error is 1 texel in the caustic map,
//    though this can be changed in the shader.
// This code creates a caustic map into the specified FBO, from the given light, using the
//    specified traversal shaders.  Rendering splats is handled by RenderSplats().
// To best understand this code, first look through adativeCaustics.cpp and the associated
//    traversal code given in utilRoutines.cpp
void AdaptiveCausticsTraversalWithErrorMetricAndBatching( int lightNum, 
														  GLSLProgram *traverse, 
														  FrameBuffer *causticMap )
{
	// Again, some prelims to make the code cleaner
	int deferredBufferWidth = data->fbo->deferredCaustic_refractorGeom->GetWidth();
	int startLevel = 6;       // Don't start from a single photon, start further down the hierarchy
	int startResolution = 64; // 2^startlevel

	// Needed due to the conflict between mipmaps "0" being high level, and "0" in our
	//    hierarchy being low resolution.
	int mipStart   = (int)(log2( deferredBufferWidth ) + 0.01) - startLevel;
	int recordLocs, currBuf=0, primCount = startResolution*startResolution;  
	int primCountAfterInitialTraversal;
	int convergedPhotonOffset = 0;
	int convergedPhotons = 0;
	int totalConvergedPhotons = 0, totalUnconvergedPhotons = 0;
	int maxConvergedPerBatch = 0, maxUnconvergedPerBatch = 0;

	// Setup state for rendering and traversal
	glPushMatrix();
	glLoadIdentity();
	scene->LightLookAtMatrix( lightNum );
	glBlendFunc( GL_ONE, GL_ONE );
	glEnableClientState( GL_VERTEX_ARRAY );
	glEnable( GL_RASTERIZER_DISCARD_NV );

	// Enable the traversal shader
	traverse->EnableShader();
	traverse->BindAndEnableTexture( "refractSurface", 
		                            data->fbo->deferredCaustic_refractorGeom->GetColorTextureID( 0 ), 
									GL_TEXTURE0, 
									data->fbo->deferredCaustic_refractorGeom->GetTextureType( FBO_COLOR0 ) );

	// Do some setup for the traversal.  Note this code must come after the traverse shader is enabled!
	recordLocs = glGetVaryingLocationNV( traverse->GetProgramID(), "gl_Position" );
	glTransformFeedbackVaryingsNV( traverse->GetProgramID(), 1, &recordLocs, GL_INTERLEAVED_ATTRIBS_NV );

		// Traverse down the hierarchy to some maximal level.  Note this is almost identical 
	    //    the initial segment in the traversal in utilRoutines.cpp, since we are travering
	    //    to a level at which we can reasonably start batching.  Additionally, it is highly
	    //    unlikely any photons will converge to a single texel (our error metric) in the first 
	    //    ~7 or 8 levels of the hierarchy, and those that do will not create an excessive 
	    //    amount of wasted work.  That means this code DOES NOT use the error metric.
		bool first = true;
		for (int i = *data->param->maxTraversalLevel - startLevel; i > 0; i--,currBuf=1-currBuf,first=false)
			primCount = TraversalPass( traverse, 
				                      first ? data->vbo->genericTraversalStartBuffer : data->vbo->genericTraversalBuffer[currBuf], 
									  i != 1 ? data->vbo->genericTraversalBuffer[1-currBuf] : data->vbo->genericTraversalBuffer[4],
									  deferredBufferWidth, mipStart--, primCount );
		primCountAfterInitialTraversal = primCount;

	// OK.  We're done traversing, so disable the "traverse" shader
	traverse->DisableTexture( GL_TEXTURE0, 
		                      data->fbo->deferredCaustic_refractorGeom->GetTextureType( FBO_COLOR0 ) );
	traverse->DisableShader();
	glPopMatrix();
	glDisable( GL_RASTERIZER_DISCARD_NV );


	// Now we split the photons into batches, traversing each batch further individually.  This code
	//    is significantly different than the code give in utilRoutines.cpp, since at each step we 
	//    must check if photons converge, and if they do output them to a final "converged photons"
	//    list.  These converged photons will not be used in later steps of the traversal, and are
	//    ready to be splatted out (as long as an appropriate intensity modifier is kept).

	
	// Now traverse adaptively further down the hierarchy, consolidating converging photons to a
	//   "converged photon list" and subdividing other photons into 4 each.
	int convergedPhotonBuf = 3, inputBuf, photonHitBuf, splitOutputBuf = currBuf;
	int primsProcessed = 0, primsPerBatch = primCountAfterInitialTraversal / (*data->ui->numBatches);

	// This code *requires* we have a multiple of 4 photons.  This is guaranteed from our traversal
	//    above, but once we divide by an arbitrary # of batches, it no longer is.  So round up to
	//    the next multiple of 4.  This just means our last batch may be 4 photons shorter.
	primsPerBatch += 4-(primsPerBatch % 4);

	// This should never occur.  But it's here unless I (again) do something stupid in the shaders.
	if (primsPerBatch % 4) printf("Warning: PrimsPerBatch not multiple of 4!\n");

	// Now do the per-batch traversal
	int mipLevelForBatch = mipStart;
	for (int batch=0; batch < *data->ui->numBatches; batch++)
	{
		// Per-batch reinitialization.
		float splatIntensity=4.0;    // A bit arbitrary, but matches with an arbitrary const in shaders.
		mipStart = mipLevelForBatch; // Every batch starts at the same level in the traversal.  Reset.
		glPushMatrix();
		glLoadIdentity();
		scene->LightLookAtMatrix( lightNum );
		glEnable( GL_RASTERIZER_DISCARD_NV );

		// Make sure we know which primitives we'll be processing from above.
		convergedPhotons = 0;
		primCount = primsPerBatch;
		if (primsProcessed+primCount > primCountAfterInitialTraversal)
			primCount = primCountAfterInitialTraversal-primsProcessed;

		// Now continue traversing until all our photons converge OR we meet some
		//    maximal traversal level.  We add this maximal, as noted in the paper,
		//    because many objects require a truly excessive # of photons to converge
		//    everywhere.  This is usually unnecessary and VERY expensive.  Plus once
		//    you have thousands of photons modifying each texel in the caustic map,
		//    precision issues during blending negate any gain you might get.
		// The <= (instead of <) allows for the difference between rendering of this
		//    implementation versus the adaptive metric with maximal traversal error,
		//    so that speeds are comparable with the same parameters.
		for (int i=0; i <= *data->ui->adaptiveDepthParam; i++)
		{
			// Set nicely-named indices to our (poorly-named temporary) buffers correctly
			inputBuf = splitOutputBuf;  photonHitBuf = (inputBuf+1)%3; 	splitOutputBuf = (inputBuf+2)%3;

			// Apply our shader that actually computes the photon hit points.  I.e., this
			//    computes photon hitpoints for all photons in the list.  For each group of
			//    4 (clustered as 2x2 texel blocks) it checks if they converge, and outputs
			//    a photon position and a flag (yes, I converge or no, I do not).
			IdentifyConvergingPhotons( i != 0 ? data->vbo->genericTraversalBuffer[inputBuf] : data->vbo->genericTraversalBuffer[4], 
							   data->vbo->genericTraversalBuffer[photonHitBuf], 
							   primCount, lightNum,
							   i != 0 ? 0 : primsProcessed*4*sizeof(float) );

			// Now, from the list above, output a sublist of photons that have fully converged.  Since
			//    we concatenate lists of converged photons, we need to pass in an offset (16*previous
			//    converged photons), and each traversal level reduces splat intensity by 1/4th.
			convergedPhotons += OutputPhotonSubList( data->shader->adaptiveCaustics_errorMetric_streamOutConverged, 
												data->vbo->genericTraversalBuffer[photonHitBuf], 
												data->vbo->genericTraversalBuffer[convergedPhotonBuf], 
												primCount, splatIntensity *= 0.25, convergedPhotons*16 );

			// Split unconverged photons into 4 children...  We don't do so at the final level, to 
			//    allow experimental traversal 1 level deeper than usual (otherwise memory issues 
			//    can cause a crash at this function after too many iterations).
			if (i+1 <= *data->ui->adaptiveDepthParam)
				primCount = ModifiedTraversalPass( data->shader->adaptiveCaustics_errorMetric_Traversal, 
													  deferredBufferWidth, mipStart--, primCount,
													  i != 0 ? data->vbo->genericTraversalBuffer[inputBuf] : data->vbo->genericTraversalBuffer[4],
													  data->vbo->genericTraversalBuffer[photonHitBuf],
													  data->vbo->genericTraversalBuffer[splitOutputBuf],
													  i != 0 ? 0 : primsProcessed*4*sizeof(float) );
		}

		// Now, create a list of as yet unconverged photons.   These are left over photons after
		//   we reach our maximal traveral (added, as discussed in the paper, to avoid excessive 
		//   subdivision)
		int unconvergedPhotons = OutputPhotonSubList( data->shader->adaptiveCaustics_errorMetric_streamOutUnconverged, 
													  data->vbo->genericTraversalBuffer[photonHitBuf], 
													  data->vbo->genericTraversalBuffer[splitOutputBuf], 
													  primCount, splatIntensity, 0 );

		// When we're ready to render, we have to re-enable rasterization
		glDisable( GL_RASTERIZER_DISCARD_NV );
		glPopMatrix();

		// Now actually splat the photons from this batch. 

		// First, we splat all the unconverged photons into a smaller buffer.
		//    Ideally, we'd splat this into a true hierarchical caustic map, with
		//    3 or 4 layers, but for now we'll just do a simple decomposition.  
		//    Unconverged photons will go into a caustic map 1/4 the resolution of 
		//    the final map.
		data->fbo->causticMapSmall->BindBuffer();
		if (batch==0) data->fbo->causticMapSmall->ClearBuffers();
			RenderSplats( data->vbo->genericTraversalBuffer[splitOutputBuf], unconvergedPhotons, data->fbo->causticMapSmall );
		data->fbo->causticMapSmall->UnbindBuffer();

		// Second, splat the converged photons into the full resolution caustic
		//    map.  We don't need to worry about noise here, since our error metric
		//    guarantees they will be converging roughly to a single texel
		causticMap->BindBuffer();
		if (batch==0) causticMap->ClearBuffers();
		RenderSplats( data->vbo->genericTraversalBuffer[convergedPhotonBuf], convergedPhotons, causticMap );
		causticMap->UnbindBuffer();

		// Done with this batch, get ready to process the next batch (if needed)
		primsProcessed += primsPerBatch;
		totalConvergedPhotons += convergedPhotons;
		totalUnconvergedPhotons += unconvergedPhotons;
		maxConvergedPerBatch = MAX( maxConvergedPerBatch, convergedPhotons );
		maxUnconvergedPerBatch = MAX( maxUnconvergedPerBatch, unconvergedPhotons );
	}
	
	// OK, now we're going to copy (with alpha blending) the low-res texture on
	//    top of the full-res caustic map, to get the final multi-res caustic
	//    map.  Again, ideally, we'd have more than 2 layers here to get rid
	//    of any remaining noise.
	causticMap->BindBuffer();
	DisplayTexture( data->fbo->causticMapSmall->GetColorTextureID() );
	causticMap->UnbindBuffer();

	glDisableClientState( GL_VERTEX_ARRAY );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
}

// Actually draw the scene with adaptively sampled photons
void Display_WithAdaptiveCausticsUsingErrorMetricAndBatching( void )
{
	float glassColor[4] = {1.0, 1.0, 1.0, 0.0};
	int lightNum = 0;

	// Generate shadow map(s) for all of the enabled lights
	for (int i=0; i < scene->NumLightsEnabled(); i++)
		scene->CreateShadowMap( data->fbo->shadowMap[i], 
								data->param->shadowMatrixTrans[i], 
								i, // Light Number
								*(data->param->shadowMapBias) );

	// Generate the caustic maps for all the enabled lights
	for (int lightNum=0; lightNum < scene->NumLightsEnabled(); lightNum++)
	{
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		scene->LightPerspectiveMatrix( lightNum, 1 );
		glMatrixMode( GL_MODELVIEW );

		// Create the buffers needed for refraction from the light's point of view.
		CreateBuffersForAdaptiveCaustics( lightNum );

		// Create the caustic map by adaptively sampling the refractive object
		AdaptiveCausticsTraversalWithErrorMetricAndBatching( lightNum, 
			                                                 data->shader->adaptCaustic_traverse,
															 data->fbo->causticMap[lightNum] );

		// All right, we just created the caustic map, so do some housekeeping to make sure
		//    our rendering pipeline (see directories "Scene," "Objects" and "Materials") can 
		//    correctly render geometry using a caustic map.

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


