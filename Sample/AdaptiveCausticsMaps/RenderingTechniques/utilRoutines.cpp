/***************************************************************************/
/* utilRoutines.cpp                                                        */
/* -----------------------                                                 */
/*                                                                         */
/* These are utility routines that may (or may not) be reused in multiple  */
/*    techniques in the various files in this directory.  However, these   */
/*    functions are not particularly relevant to the renderings, but       */
/*    instead encapsulate annoying OpenGL code that would otherwise make   */
/*    the rendering code difficult to read.                                */
/*                                                                         */
/* Chris Wyman (02/23/2008)                                                */
/***************************************************************************/

#include "causticDemo.h"
#include "renderingData.h"
#include "renderingMethods.h"

extern Scene *scene;
extern RenderingData *data;


// Start counting the number of primitives output.
void BeginPrimitiveCount( GLenum primType, GLuint queryID )
{
	glBeginTransformFeedbackNV( primType );
	glBeginQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV, queryID );
}

// OK, we're done drawing, return the number of primitives we drew.
//    Technically, this won't work, since glGetQueryObjectiv() is
//    not guaranteed to return the correct number until a "true"
//    flag is set saying the value is available.  However, on three
//    different nVidia cards (8800 GTX, 9800x2, GTX 280) this is
//    unnecessary and adds significant overhead.  Given that
//    a new extension NV_transform_feedback2 removes the requirement
//    to even do this primitive-count query, I don't think this
//    failure to follow spec is a big deal.
int EndPrimitiveCount( GLuint queryID )
{
	GLint primCount;
	glEndQuery( GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN_NV );
	glEndTransformFeedbackNV();
	glGetQueryObjectiv( queryID, GL_QUERY_RESULT, &primCount );
	return primCount;
}

// Set the light matrices for a given light to render into
//    a FBO with a specified aspect ratio.  This must be
//    followed by a PopLightMatrices(), since there's a push
//    hidden inside here.
void SetLightMatrices( int lightNum, float aspect )
{
	GLLight *light = scene->GetLight( lightNum );
	Camera *camera = scene->GetCamera();

	// Do some computations to get the light position, aspect, view direction and up.
	Point lightPos( light->GetCurrentPos() );
	Point at( camera->GetAt() );
	Vector view = at-lightPos;
	view.Normalize();
	Vector perp = abs( view.Dot( Vector::YAxis() ) ) < 0.95 ? view.Cross( Vector::YAxis() ) : view.Cross( Vector::XAxis() );
	perp.Normalize();
	Vector up = perp.Cross( view );

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluPerspective( light->GetLightFovy(), aspect, 
					light->GetLightNearPlane(), light->GetLightFarPlane() );
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	gluLookAt( lightPos.X(), lightPos.Y(), lightPos.Z(), 
			   at.X(), at.Y(), at.Z(), 
			   up.X(), up.Y(), up.Z() );
}

// Pop the light matrices specified by SetLightMatrices()
void PopLightMatrices( void )
{
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

// A silly square function
float sqr( float f )
{
	return f*f;
}

// A simple log-base2 function
// float log2( float x )
// {
// 	return log(x)/log(2.0);
// }

// Copies both the color and the depth buffer from a prior texture into another.
//    We use this to avoid 2 renders of the opaque geometry seen from the eye.
//    This needs to be rendered once for use during refraction and once in the
//    final image.  Ideally we could use the same buffer, without this copy,
//    but that would lead to read-modify-write hazards that GPUs don't handle.
void CopyColorAndDepthTexturesIntoCurrentFBO( GLuint colorTex, GLuint depthTex )
{
	int minFilter1, magFilter1, minFilter2, magFilter2;
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,1,0,1);
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable( GL_LIGHTING );

	data->shader->copyColorAndDepth->EnableShader();
	data->shader->copyColorAndDepth->BindAndEnableTexture( "colorTex", colorTex, GL_TEXTURE0 );
	glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter1 );
	glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter1 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	data->shader->copyColorAndDepth->BindAndEnableTexture( "depthTex", depthTex, GL_TEXTURE1 );
	glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter2 );
	glGetTexParameteriv( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter2 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glColor3f( 1, 1, 1 );
	glBegin( GL_QUADS );
		glTexCoord2f(0,0);	glVertex2f(0,0);
		glTexCoord2f(1,0);	glVertex2f(1,0);
		glTexCoord2f(1,1);	glVertex2f(1,1);
		glTexCoord2f(0,1);	glVertex2f(0,1);
	glEnd();

	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter2 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter2 );
	data->shader->copyColorAndDepth->DisableTexture( GL_TEXTURE1 );
	glActiveTexture( GL_TEXTURE0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter1 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter1 );
	data->shader->copyColorAndDepth->DisableTexture( GL_TEXTURE0 );
	data->shader->copyColorAndDepth->DisableShader();
	
	glPopAttrib();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

// Set the binding of the VBO to the specified one, and set
//   the vertex pointer to the specified offset.
void SetCurrentVertexBuffer( GLuint vboID, GLuint offset )
{
	glBindBuffer( GL_ARRAY_BUFFER, vboID );      
	glVertexPointer( 4, GL_FLOAT, 0, BUFFER_OFFSET( offset ) );  
}


// Functions needed inside the hierarchical traversal code below.  These
//   are defined below the traversal.
void EnableStandardTraversal( GLSLProgram *traverse );
void DisableStandardTraversal( GLSLProgram *traverse );
int StandardTraversalPass( GLSLProgram *traverse, GLuint inputBuffer, GLuint outputBuffer,
				           int deferredBufferWidth, int mipLevel, int inputPrims, int inputOffsetBytes=0 );
void RenderStandardSplats( GLSLProgram *render, GLuint inputBuffer, int lightNum, int inputPrims, int addedDepth=0 );


// Traverses down a hierarchy in a breadth-first search (with batching to
//    split traversal into separate pieces).  This is used in the simple
//    adaptive caustic mapping using image-space splatting (adaptiveCaustics.cpp)
//    that uses a maximum-traversal metric.  Unfortunately, the maximum-error metric
//    requires much more complex traversal code (shown in 
//    adaptiveCausticsErrorMetricWithBatching.cpp).
void HierarchicalTransformFeedbackTraversalWithBatching( GLSLProgram *traverse,
											             GLSLProgram *render,
											             int startLevel,
											             int lightNum )
{
	// Setup shortcuts to make code more readable
	GLuint *travBuf = data->vbo->genericTraversalBuffer;
	GLuint startBuf = data->vbo->genericTraversalStartBuffer;

	// We have a specific resolutionlevel to start at (64x64, 2^6 x 2^6, or "level" 6).  
	//   Unfortunately, mipmaps count "0" as the highest resolution, and we count "0" 
	//   as a 1 x 1 image, so we need this computation to get the correct mipmap level 
	//   for our shader.  This is then decremented (because we increase resolution) 
	//   each step as we traverse.
	int decrCounter   = (int)(log2( data->fbo->deferredCaustic_refractorGeom->GetWidth() ) + 0.01) - startLevel; 

	// What resolution is the start of our traversal at?  We'll have 
	//   startResolution*startResolution photon clusters to process during
	//   the first traversal step.
	int startResolution = (int)( pow( 2.0f, (float)startLevel ) + 0.5 );
	
	// We do ping-ponging back and forth between two temporary vertex buffers
	//   that store our intermediate photons.  To guarantee we always end with
	//   out last pass output to the SAME BUFFER, no matter the # of traversals,
	//   we compute which buffer we should start with.
	int currBuf = 1-(*data->param->maxTraversalLevel - startLevel)%2; // Computed so last standard traversal outputs travBuf[0] 

	// A primitive count temporary we'll reuse a lot.
	int primCount = 0;

	// Tell OpenGL we'll be using some vertex arrays to specify photon data
	glEnableClientState( GL_VERTEX_ARRAY );	    

	// Traverse down the hierarchy.  First enable the traversal.  Than for each step,
	//    use the output from the prior level as a list of photons, check if they're
	//    valid and need to be subdivided to a finer level and then output them.  The
	//    first pass takes a fixed size (say 64*64) grid of regularly sampled point
	//    on the rage [0..1] to sample the image.  Finally, once we have traversed to
	//    the maximal level, disable the shader and move on.
	EnableStandardTraversal( traverse );
	for (int stepCount = 0; stepCount < *data->param->maxTraversalLevel - startLevel; stepCount++,currBuf=1-currBuf)
		primCount = StandardTraversalPass( traverse, 
			                               stepCount > 0 ? travBuf[1-currBuf] : startBuf,
			                               travBuf[currBuf], 
										   data->fbo->deferredCaustic_refractorGeom->GetWidth(), 
										   decrCounter--, 
										   stepCount > 0 ? primCount : startResolution * startResolution );
	DisableStandardTraversal( traverse );

	// One way to do "batching," as described in the paper, would be to split the fixed size
	//   regular sampling into batches and process them individually.  This has a number of
	//   problems (one being that some batches might then have no valid photons, which nullifies
	//   some advantages of splitting photons in the first place).  To avoid this problems, we
	//   do an initial traversal (above) to some intermediate level, and then split the *resulting*
	//   photons into batches.  This means we already know the photons are valid (or barely missed
	//   the refractor), avoiding uneven distributions of photons in the batches.  A secondary
	//   refinement process (below) then subdivides these photons further.

	// The input buffer, containing all photons output from above (used as input below)
	int unbatchedPhotonBuf = 0; // (or 1-currBuf, which should always be 0 here)

	// A number of output prims from above, which are split into (roughly) equal patches
	int primCountAfterInitialTraversal = primCount;
	int primsPerBatch = primCountAfterInitialTraversal / (*data->ui->numBatches);

	// Keep a counter of how many photons we've processed.
	int initialPrimsProcessed = 0;

	// This is that same odd mipmap level we used above, which is decreased every step
	//    during the traversal.  However, it has to be reset for each new batch, so we 
	//    need a second counter.
	int counterLevelForBatch = decrCounter;  
	for (int batch=0; batch < *data->ui->numBatches; batch++)
	{
		// For each batch, reset some traversal parameters, and update the primitive count.
		currBuf = 0;                         // Unbatched photons stored in travBuf[0]
		decrCounter = counterLevelForBatch;  // Reset decrCounter for the next batch
		primCount = primsPerBatch;           // How many prims to process?  The number in each batch!

		// Make sure we don't accidentally process too many photons (and run off the 
		//    end of our memory)
		if (initialPrimsProcessed+primsPerBatch > primCountAfterInitialTraversal)
			primCount = primCountAfterInitialTraversal-initialPrimsProcessed;

		// Adaptively traverse further for the photons in this batch.  This uses the same
		//   process (and code) as above, except the parameter buffers and initial input
		//   are different.
		EnableStandardTraversal( traverse );
		for (int stepCount=0; stepCount < *data->ui->adaptiveDepthParam; stepCount++,currBuf=1-currBuf)
		{
			primCount = StandardTraversalPass( traverse, 
			                                   stepCount > 0 ? travBuf[1+(1-currBuf)] : travBuf[unbatchedPhotonBuf],
			                                   travBuf[1+(currBuf)], 
										       data->fbo->deferredCaustic_refractorGeom->GetWidth(), 
										       decrCounter--, 
										       primCount,
											   stepCount > 0 ? 0 : initialPrimsProcessed*4*sizeof(float) );
		}
		DisableStandardTraversal( traverse );

		// OK, now that we've subdivided the photons in the batch to their maximum level,
		//    splat them out into the caustic map.
		RenderStandardSplats( render, 
							  travBuf[1+(1-currBuf)], 
						      lightNum, 
						      primCount,
							  *data->ui->adaptiveDepthParam );

	    // Cleanup before the next batch
	    initialPrimsProcessed += primsPerBatch;
	}

	// Clean up OpenGL state
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableClientState( GL_VERTEX_ARRAY );
}

// This enables the traversal shader (specified as input) and sets the
//    parameters necessary for caustic traversal (namely the two-layer
//    refraction normals -- other parameters are enabled automatically 
//    by the shader class).  Additionally, it also makes sure the
//    NV_transform_feedback is enabled and bound to the appropriate
//    geometry shader output.
void EnableStandardTraversal( GLSLProgram *traverse )
{
	// Enable the shader
	traverse->EnableShader();
	FrameBuffer *inputBuf  = data->fbo->deferredCaustic_refractorGeom;
	traverse->BindAndEnableTexture( "refractSurface", 
		                            inputBuf->GetColorTextureID( 0 ), 
									GL_TEXTURE0, 
									inputBuf->GetTextureType( FBO_COLOR0 ) );

	// Setup transform feedback to allow readback of the positions output
	int posLoc = glGetVaryingLocationNV( traverse->GetProgramID(), "gl_Position" );
	glTransformFeedbackVaryingsNV( traverse->GetProgramID(), 1, &posLoc, GL_INTERLEAVED_ATTRIBS_NV );

	// Don't rasterize these...
	glEnable( GL_RASTERIZER_DISCARD_NV );
}

// Do the opposite of the EnableStandardTraversal().
void DisableStandardTraversal( GLSLProgram *traverse )
{
	// Disable raterization discards
	glDisable( GL_RASTERIZER_DISCARD_NV );

	// Disable the shader
	FrameBuffer *inputBuf  = data->fbo->deferredCaustic_refractorGeom;
	traverse->DisableTexture( GL_TEXTURE0, inputBuf->GetTextureType( FBO_COLOR0 ) );
	traverse->DisableShader();
}

// This performs a single render pass (using the specified shader) over all the 
//    photons specified in the input buffer, outputs them to the specified output
//    buffer.  deferredBufferWidth is the resolution of the deferred surface normal
//    buffer, which is needed together with the mipLevel to determine if we index 
//    into a mipmap layer of the deferred buffer or if we are interpolating between
//    texels at the highest resolution.  The inputPrims specifies how many photons
//    we need to process this pass, and the optional final parameter gives an offset
//    into the buffer for where to start processing (useful for batching).  The 
//    return value gives how many photons were output to the outputBuffer.
int StandardTraversalPass( GLSLProgram *traverse, GLuint inputBuffer, GLuint outputBuffer,
				           int deferredBufferWidth, int mipLevel, int inputPrims, int inputOffsetBytes )
{
	int resolution;

	// Set our output buffer
	glBindBufferBaseNV( GL_TRANSFORM_FEEDBACK_BUFFER_NV, 0, outputBuffer );
	BeginPrimitiveCount( GL_POINTS, data->glID->primCountQuery  );

	// Set our input buffer
	glBindBuffer( GL_ARRAY_BUFFER, inputBuffer );    
	glVertexPointer( 4, GL_FLOAT, 0, BUFFER_OFFSET( inputOffsetBytes ) );  

		// Figure out if we're using mipmap data or interpolating between texels at the
	    //    full resolution image of the geometry buffers.
		if (mipLevel>0) 
			resolution = deferredBufferWidth / (int)( pow( 2.0f, (float)mipLevel ) + 0.5 ); 
		else
			resolution = deferredBufferWidth * (int)( pow( 2.0f, (float)-mipLevel ) + 0.5 ); 

		// How far apart are texels at our current mipmap/traversal level?
		traverse->SetParameter( "deltaHierRes", 0.5f/resolution );    

		// What level are we at in the hierarchy?
		traverse->SetParameter( "currHierarchyLevel", (float)mipLevel );

		// Go ahead and draw
		glDrawArrays( GL_POINTS, 0, inputPrims );  

	// Return the number of primitives (photons) output in this pass
	return EndPrimitiveCount( data->glID->primCountQuery  );
}

// This takes a vertex buffer full of a specified number of photons, and renders them using
//    the specified shader into the current framebuffer.  The lightNum is needed to get the
//    correct viewing matrices.
void RenderStandardSplats( GLSLProgram *render, GLuint inputBuffer, int lightNum, int inputPrims, int addedDepth )
{
	// Mostly this is ugly setup of the parameters and textures for the shader.
	SetCurrentVertexBuffer( inputBuffer );    
	render->EnableShader(); 

	// What level are these photons rendered at?  This is really important to get the 
	//    intensity right.  Splats from a coarse sampling have more energy per photon
	//    than splats at a finer sampling.
	int maxPhotonRes = (int)(pow(2.0,*data->param->maxTraversalLevel+addedDepth)+0.1);

	// Set up the refraction parameters
	float idx = *data->param->indexOfRefraction;
	float _near = scene->GetLight( lightNum )->GetLightNearPlane(), _far = scene->GetLight( lightNum )->GetLightFarPlane();
	render->SetParameter( "local1", _near*_far, _far-_near, _far+_near, _far); 
	render->SetParameter( "local2", 1.0/idx, 1.0/(idx*idx), idx, idx*idx );

	// Some matrix / viewing data so we know where to draw specified points in the caustic map
	render->Set4x4MatrixParameterv( "gluOrtho", data->param->gluOrtho );
	render->SetParameter( "splatResolutionModifier", sqr( data->fbo->causticMap[lightNum]->GetWidth()/(float)maxPhotonRes ) ); //data->fbo->deferredCaustic_refractorGeom->GetWidth() );
	render->SetParameter( "renderBufRes", data->fbo->causticMap[lightNum]->GetWidth() );
	render->SetParameter( "tanLightFovy2", tan( M_PI * scene->GetLight( lightNum )->GetLightFovy() / 360.0 ) );
	render->Set4x4MatrixParameterv( "shadowMatrix", scene->GetShadowMapMatrix( lightNum ) );
	
	// Geometry buffers for deferred refraction. 
	render->BindAndEnableTexture( "geomNorm", data->fbo->deferredCaustic_refractorGeom->GetColorTextureID( 0 ), GL_TEXTURE1, GL_TEXTURE_2D_ARRAY_EXT );
	glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	render->BindAndEnableTexture( "geomDepth", data->fbo->deferredCaustic_refractorGeom->GetDepthTextureID(), GL_TEXTURE2, GL_TEXTURE_2D_ARRAY_EXT );
	glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// Here we do the drawing
	glDrawArrays( GL_POINTS, 0, inputPrims ); 

	// Ugly disable/cleanup code
	render->DisableTexture( GL_TEXTURE2, GL_TEXTURE_2D_ARRAY_EXT );
	render->DisableTexture( GL_TEXTURE1, GL_TEXTURE_2D_ARRAY_EXT );
	//render->DisableTexture( GL_TEXTURE0, GL_TEXTURE_2D_ARRAY_EXT );
	render->DisableShader();
}


// Since we do not have a "render-to-mipmap" functionality on GPUs,
//    we render to EXT_texture_array, using different parts of each
//    texture array.  This render pass takes the array texture and
//    recombines it into a single image by rescaling as appropriate.
//    (This gives our recombined caustic map using Hierarchical 
//    Caustic Maps.)  It is significantly quicker to do this as a
//    separate pass than once at every pixel that needs to look up
//    a caustic contribution.
void CombineMipmapArrayLevels( GLuint arrayTex )
{
	int minFilter, magFilter;
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,1,0,1);
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable( GL_LIGHTING );

	// A special shader does this computation over a screen/image-sized input.
	data->shader->combineMipmapArrayLevels->EnableShader();
	data->shader->combineMipmapArrayLevels->BindAndEnableTexture( "textureArray", arrayTex, GL_TEXTURE0, GL_TEXTURE_2D_ARRAY_EXT );
	glGetTexParameteriv( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, &minFilter );
	glGetTexParameteriv( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, &magFilter );
	glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	glBegin( GL_QUADS );
		glTexCoord2f(0,0);	glVertex2f(0,0);
		glTexCoord2f(1,0);	glVertex2f(1,0);
		glTexCoord2f(1,1);	glVertex2f(1,1);
		glTexCoord2f(0,1);	glVertex2f(0,1);
	glEnd();

	glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MIN_FILTER, minFilter );
	glTexParameteri( GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_MAG_FILTER, magFilter );
	data->shader->combineMipmapArrayLevels->DisableTexture( GL_TEXTURE0, GL_TEXTURE_2D_ARRAY_EXT );
	data->shader->combineMipmapArrayLevels->DisableShader();
	
	glPopAttrib();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

// A simple utility to display a texture as a full-screen image.
void DisplayTexture( GLuint texID )
{
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,1,0,1);
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable( GL_LIGHTING );
	glEnable( GL_BLEND );
	glDisable( GL_DEPTH_TEST );
	glActiveTexture( GL_TEXTURE0 );
	glBindTexture( GL_TEXTURE_2D, texID );
	glEnable( GL_TEXTURE_2D );
	glColor4f( 1, 1, 1, 1 );
	glBegin( GL_QUADS );
		glTexCoord2f(0,0);	glVertex2f(0,0);
		glTexCoord2f(1,0);	glVertex2f(1,0);
		glTexCoord2f(1,1);	glVertex2f(1,1);
		glTexCoord2f(0,1);	glVertex2f(0,1);
	glEnd();
	glDisable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, 0 );
	glPopAttrib();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
}

// This function takes the depth buffer from the given input FBO and
//   copies it to the output (cMap) framebuffer, without modifying the
//   color buffer of the output.
void CopyDepthToCausticMap( FrameBuffer *cMap, FrameBuffer *inputDepth )
{
	cMap->BindBuffer();
	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,1,0,1);
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glDepthMask( GL_TRUE );
	cMap->ClearBuffers();
	data->shader->copyDepth->EnableShader();
	data->shader->copyDepth->BindAndEnableTexture( "depthTex", inputDepth->GetDepthTextureID(), 
		                            GL_TEXTURE0, inputDepth->GetTextureType( FBO_DEPTH ) );
	glColor4f( 1, 1, 1, 1 );
	glBegin( GL_QUADS );
		glTexCoord2f(0,0);	glVertex2f(0,0);
		glTexCoord2f(1,0);	glVertex2f(1,0);
		glTexCoord2f(1,1);	glVertex2f(1,1);
		glTexCoord2f(0,1);	glVertex2f(0,1);
	glEnd();
	data->shader->copyDepth->DisableTexture( GL_TEXTURE0, inputDepth->GetTextureType( FBO_DEPTH ) );
	glPopAttrib();
	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();
	data->shader->copyDepth->DisableShader();
	cMap->UnbindBuffer();
}




// Sets up a prerendered help screen.  Drawing text with GLUT seems to be 
//    quite slow, so we avoid doing it every frame, and instead only do it
//    once (or once every time parameters change, in our case).
void SetupHelpScreen( FrameBuffer *helpFB )
{
	helpFB->BindBuffer();
	glClearColor( 0, 0, 0, 0 );
	helpFB->ClearBuffers();
	glClearColor( 0, 0, 0, 1 );
	
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	glDisable( GL_NORMALIZE );
	glDisable( GL_CULL_FACE );

	glColor4f(0.2, 0.2, 0.2, 0.8);
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0,1,0,1);
		glMatrixMode( GL_MODELVIEW );
		glPushMatrix();
		glLoadIdentity();
		glBegin( GL_QUADS );
			glTexCoord2f(0,0);	glVertex2f(0.05,0.05);
			glTexCoord2f(1,0);	glVertex2f(0.95,0.05);
			glTexCoord2f(1,1);	glVertex2f(0.95,0.95);
			glTexCoord2f(0,1);	glVertex2f(0.05,0.95);
		glEnd();

		glActiveTexture( GL_TEXTURE0 );
		glEnable( GL_TEXTURE_2D );
		glEnable( GL_BLEND );
		glBlendFuncSeparate( GL_ONE, GL_ONE, GL_ZERO, GL_ONE );
		glBindTexture( GL_TEXTURE_2D, data->tex->iowaLogo->TextureID() );
		glBegin( GL_QUADS );
			glTexCoord2f(0.01,0.01);	glVertex2f(0.8,0.07);
			glTexCoord2f(0.99,0.01);	glVertex2f(0.93,0.07);
			glTexCoord2f(0.99,0.99);	glVertex2f(0.93,0.2);
			glTexCoord2f(0.01,0.99);	glVertex2f(0.8,0.2);
		glEnd();
		glBindTexture( GL_TEXTURE_2D, 0 );
		glDisable( GL_BLEND );
		glDisable( GL_TEXTURE_2D );

		glMatrixMode( GL_PROJECTION );
		glPopMatrix();
		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();
		
	glColor4f(1,1,0,1);
	    DisplayString( 75, 950, "User Interface Commands:", 1024, 1024 );
		DisplayString( 570, 60, "More Information: http://www.cs.uiowa.edu/~cwyman/publications/" );
		DisplayString( 570, 105, "\"Adaptive Caustic Maps Using Deferred Shading\"" );
		DisplayString( 625, 90, "Chris Wyman and Greg Nichols" );
		DisplayString( 617, 75, "Proceedings of Eurographics 2009" );
	glColor4f(1,1,1,1);
		DisplayString( 120, 920, "Toggle on / off this help screen", 1024, 1024 );
		DisplayString( 120, 900, "Toggle on / off scene animation", 1024, 1024 );
		DisplayString( 120, 880, "Capture screenshot", 1024, 1024 );
		DisplayString( 120, 860, "Reload shaders (Try reloading if your driver had errors during shader compilation)", 1024, 1024 );
		DisplayString( 120, 840, "Quit demonstration program", 1024, 1024 );

	glColor4f(0,1,1,1);
		DisplayString( 75, 920, "[ h ]", 1024, 1024 );
		DisplayString( 75, 900, "[ a ]", 1024, 1024 );
		DisplayString( 75, 880, "[ f12 ]", 1024, 1024 );
		DisplayString( 75, 860, "[ r ]", 1024, 1024 );
		DisplayString( 75, 840, "[ q ]", 1024, 1024 );
		DisplayString( 75, 800, "[ + ] and [ - ]", 1024, 1024 );
		DisplayString( 75, 780, "[ l ] and [ L ]", 1024, 1024 );
		DisplayString( 75, 760, "[ . ] or [ , ]", 1024, 1024 );
		DisplayString( 75, 740, "[ b ] or [ B ]", 1024, 1024 );

		DisplayString( 75, 700, "[ ] ] or [ [ ]", 1024, 1024 );
		DisplayString( 75, 680, "[ ' ] or [ ; ]", 1024, 1024 );

		DisplayString( 75, 640, "[ left-mouse ]", 1024, 1024 );
		DisplayString( 75, 620, "[ middle-mouse ]", 1024, 1024 );
		DisplayString( 75, 600, "[ right-mouse ]", 1024, 1024 );
		DisplayString( 75, 580, "[ shift + right-mouse ]", 1024, 1024 );

		DisplayString( 275, 220, "[ NumPad / ] and [ NumPad * ]", 1024, 1024 );
		DisplayString( 520, 310, "(see paper)", 1024, 1024 );
		DisplayString( 520, 295, "(Wyman & Davis, I3D 2006)", 1024, 1024 );
		DisplayString( 520, 280, "(see paper)", 1024, 1024 );
		DisplayString( 520, 265, "(Wyman, I3D 2008)", 1024, 1024 );
		DisplayString( 520, 250, "(see paper)", 1024, 1024 );

	glColor4f(1,1,1,1);
		DisplayString( 180, 800, "Increase or decrease index of refraction", 1024, 1024 );
		DisplayString( 180, 780, "Add or remove additional light sources (up to maximum of 2, as specified in the scene file)", 1024, 1024 );

		DisplayString( 180, 760, "Increase or decrease the light's intensity", 1024, 1024 );
		DisplayString( 180, 740, "Increase or decrease the number of photon batches (important if using more than 4096 x 4096 photons)", 1024, 1024 );

		DisplayString( 180, 700, "Increase or decrease photon count (before splitting into batches)", 1024, 1024 );
		DisplayString( 180, 680, "Increase or decrease photon count (after splitting into batches)", 1024, 1024 );

		DisplayString( 200, 640, "Rotate eye point around dragon using a trackball", 1024, 1024 );
		DisplayString( 200, 620, "Rotate glass object using a trackball", 1024, 1024 );
		DisplayString( 200, 600, "Rotate light #0 using a trackball", 1024, 1024 );
		DisplayString( 200, 580, "Rotate light #1, if enabled, using a trackball", 1024, 1024 );

		DisplayString( 75, 340, "Six rendering modes are enabled in this demo program:", 1024, 1024 );
		DisplayString( 100, 325, "1) Standard diffuse OpenGL Rendering", 1024, 1024 );
		DisplayString( 100, 310, "2) Deferred image-space refraction..........................................................................", 1024, 1024 );
		DisplayString( 100, 295, "3) Caustics via image-space splatting......................................................................", 1024, 1024 );
		DisplayString( 100, 280, "4) Adaptive caustics via image-space splatting, maximal traversal metric.....", 1024, 1024 );
		DisplayString( 100, 265, "5) Hierarchical caustic maps.......................................................................................", 1024, 1024 );
		DisplayString( 100, 250, "6) Adaptive hierarhical caustic maps, maximal caustic error metric..................", 1024, 1024 );
		DisplayString( 75, 220, "Switch between these modes using", 1024, 1024 );

	glColor4f(1,1,1,1);
		DisplayString( 75, 160, "Current Parameters:", 1024, 1024 );
		DisplayString( 100, 145, "Number of Lights Enabled:", scene->GetWidth(), scene->GetHeight() );
		DisplayString( 100, 130, "Number of Photon Batches:", scene->GetWidth(), scene->GetHeight() );
		DisplayString( 100, 115, "Traversal Level (Photons Used):", scene->GetWidth(), scene->GetHeight() );
		DisplayString( 100, 100, "Fixed Photon Buffer Size: ", scene->GetWidth(), scene->GetHeight() ); 
		DisplayString( 100, 85, "Caustic Map Size: ", scene->GetWidth(), scene->GetHeight() ); 
		DisplayString( 100, 70, "Index of Refraction:", scene->GetWidth(), scene->GetHeight() );

	glColor4f(1,1,0,1);
		char buf[1024];
		sprintf( buf, "%d", scene->NumLightsEnabled() );
		DisplayString( 300, 145, buf, scene->GetWidth(), scene->GetHeight() );
		int trav = *(data->param->maxTraversalLevel);
		int res = pow(2.0, trav+*data->ui->adaptiveDepthParam);
		sprintf( buf, "%d + %d (%d x %d)", trav, *data->ui->adaptiveDepthParam, res, res );
		DisplayString( 300, 115, buf, scene->GetWidth(), scene->GetHeight() );
		sprintf( buf, "%d", *data->ui->numBatches );
		DisplayString( 300, 130, buf, scene->GetWidth(), scene->GetHeight() );
		sprintf( buf, "%d x %d", data->fbo->caustic_PhotonBuffer->GetWidth(), data->fbo->caustic_PhotonBuffer->GetHeight() );
		DisplayString( 300, 100, buf, scene->GetWidth(), scene->GetHeight() );
		sprintf( buf, "%d x %d", data->fbo->causticMap[0]->GetWidth(), data->fbo->causticMap[0]->GetHeight() );
		DisplayString( 300, 85, buf, scene->GetWidth(), scene->GetHeight() );
		sprintf( buf, "%f", *data->param->indexOfRefraction );
		DisplayString( 300, 70, buf, scene->GetWidth(), scene->GetHeight() );

	glPopAttrib();
	glColor4f(1,1,1,1);
	helpFB->UnbindBuffer();

	// We've completed updating the help screen!
	data->ui->updateHelpScreen = false;
}

// Displays the already created help screen
void DisplayHelpScreen( FrameBuffer *helpFB )
{
	glPushAttrib( GL_ALL_ATTRIB_BITS );
	glEnable(GL_BLEND);
	glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
	glDisable( GL_DEPTH_TEST );
	helpFB->DisplayAsFullScreenTexture( FBO_COLOR0 );
	glPopAttrib();
}

