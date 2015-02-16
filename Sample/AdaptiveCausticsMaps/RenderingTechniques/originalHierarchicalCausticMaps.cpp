/***************************************************************************/
/* hierarchicalCausticMaps.cpp                                             */
/* -----------------------                                                 */
/*                                                                         */
/* Draw the scene with refractive objects and hierarchical caustics using  */
/*   the method proposed in the 2008 I3D paper 'Hierarchical Caustic Maps' */
/*                                                                         */
/* Chris Wyman (02/23/2008)                                                */
/***************************************************************************/

#include "causticDemo.h"
#include "renderingData.h"
#include "renderingMethods.h"

extern Scene *scene;
extern RenderingData *data;
extern FrameGrab *frameGrab;

// Some common refraction routines
void CreateRefractiveNormalMaps( int );             
void CreateBackgroundTex( FrameBuffer *fb, bool withCaustics );
void NonDeferredRefractionFromEyeView( float *glassColor );

// Given a photon buffer and its mipmap-chain, traverse the mipmap pyramid from
//    lowest to highest levels in a breadth-first manner, discarding large chunks
//    of photons that all miss the refractor (and thus do not contribute to the
//    caustics)
void HierarchialLightSpaceGather( int drawBufWidth ) 
{
	// Some random iteration variables that just feel cleaner up here.
	int posLoc, mult=1, currBuf=0, primCount = 0;

	// Traversal start shouldn't really be hard coded, but during
	//   the writing of the Hierarchical Caustic Maps paper, 2^6 (64x64)
	//   was sufficient, and in this implementation we hard coded it to
	//   simplify (i.e., reduce) the number of dynamic parameters exposed
	//   via UI.
	int traversalStartLevel = 6;
	int traversalStartResolution = 64;

	// Some renamings to make the code below cleaner and easer to read
	FrameBuffer *photonBuf = data->fbo->caustic_PhotonBuffer;
	GLSLProgram *traverse  = data->shader->caustic_TraversePhotonHier;
	GLSLProgram *splat     = data->shader->caustic_SplatPhotonHier;
	int causticMipLevels   = (int)(log2( photonBuf->GetWidth() ) + 0.01);
        
	// We'll be storing our photons in a vertex array.  Let GL know that's our input.
	glEnableClientState( GL_VERTEX_ARRAY );	    

	// Enable our traversal shader and set up our parameters (namely pass in the photon buffer)
	traverse->EnableShader();
	traverse->BindAndEnableTexture( "photonPositions", photonBuf->GetColorTextureID(), GL_TEXTURE0 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	// Traverse the first level in the photon buffer, counting the number of valid "photons"
	
	// First setup the NV_transform_feedback to capture the output photons
	posLoc = glGetVaryingLocationNV( traverse->GetProgramID(), "gl_Position" );
	glTransformFeedbackVaryingsNV( traverse->GetProgramID(), 1, &posLoc, GL_INTERLEAVED_ATTRIBS_NV );
	glEnable( GL_RASTERIZER_DISCARD_NV );
	glBindBufferBaseNV( GL_TRANSFORM_FEEDBACK_BUFFER_NV, 0, 
		                data->vbo->genericTraversalBuffer[currBuf] );

	// Now actually do the first traversal step (setting some traversal params on a per-step basis)
	BeginPrimitiveCount( GL_POINTS, data->glID->primCountQuery  );
		SetCurrentVertexBuffer( data->vbo->genericTraversalStartBuffer );    
		traverse->SetParameter( "deltaHierRes", 0.5f/traversalStartResolution ); 
		traverse->SetParameter( "currHierarchyLevel", 
								(float)( causticMipLevels - traversalStartLevel ) ); 
		glDrawArrays( GL_POINTS, 0, sqr( traversalStartResolution ) );  // Draw!
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
	primCount = EndPrimitiveCount( data->glID->primCountQuery  );

	// Continue traversing down the hierarchical photon buffer, counting number of photons
	for (int i = causticMipLevels - traversalStartLevel - 1; i > 1; i--)
	{
		// Our resolution doubles each step
		mult *= 2;

		// We're ping-ponging back and forth between two vertex buffer objects as
		//   input/output, so select the correct one for the current traversal step
		glBindBufferBaseNV( GL_TRANSFORM_FEEDBACK_BUFFER_NV, 0, 
			                data->vbo->genericTraversalBuffer[1-currBuf] );

		// Now actually do the i-th traversal step.
		BeginPrimitiveCount( GL_POINTS, data->glID->primCountQuery  );
			SetCurrentVertexBuffer( data->vbo->genericTraversalBuffer[currBuf] );    
			traverse->SetParameter( "deltaHierRes", 0.5f/(traversalStartResolution*mult) ); 
			traverse->SetParameter( "currHierarchyLevel", (float)i ); 
			glDrawArrays( GL_POINTS, 0, primCount );  
		primCount = EndPrimitiveCount( data->glID->primCountQuery  );
		currBuf = 1-currBuf;
	}
	traverse->DisableShader();

	// Now, we've traversed down the entire photon buffer hierarchy and have a list
	//     of the important photons in our VBO.  Now we need to actually draw these 
	//     photons into the caustic map! 
	glDisable( GL_RASTERIZER_DISCARD_NV );
	SetCurrentVertexBuffer( data->vbo->genericTraversalBuffer[ currBuf] );    
	
	// Enable our splatting shader.
	splat->EnableShader();              
	splat->SetParameter( "causticMapRes", drawBufWidth, 1.0f/drawBufWidth ); 
	splat->SetParameter( "photonRes", photonBuf->GetWidth(), 1.0f/photonBuf->GetWidth() ); 
	splat->SetParameter( "photonResolutionMultiplier", sqr( drawBufWidth/(float)photonBuf->GetWidth() ) );
	splat->BindAndEnableTexture( "photonPositions", photonBuf->GetColorTextureID(), GL_TEXTURE0 ); 
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	splat->BindAndEnableTexture( "lightColor", data->tex->spotlight->TextureID(), GL_TEXTURE1 );
	glEnable(GL_BLEND);                                        
	glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);       
	glBlendEquation(GL_FUNC_ADD); 
	glEnable( GL_VERTEX_PROGRAM_POINT_SIZE_ARB );	

	// Draw the photons
	glDrawArrays( GL_POINTS, 0, primCount );  

	// Clean up
	glDisable( GL_VERTEX_PROGRAM_POINT_SIZE_ARB );
    glDisable(GL_BLEND); 
	splat->DisableTexture( GL_TEXTURE1, GL_TEXTURE_2D );
	splat->DisableTexture( GL_TEXTURE0, GL_TEXTURE_2D );
	splat->DisableShader();
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glDisableClientState( GL_VERTEX_ARRAY );
} 


//  This routine computes our HCM for the specified light.
void ComputeHierarchicalCausticMapForLightNumber( int lightNum )
{
	GLenum bufs[2] = { GL_COLOR_ATTACHMENT0_EXT, GL_COLOR_ATTACHMENT1_EXT };

	// Either 1 or 0, depending on the surface we're using to approximate
	//   our refraction.  (If we use surfaces obtained with a single depth 
	//   peel (1) or surfaces furthest from the light (0))
	float frontSide = 1.0;

	// Compute the back-facing normal map, as seen from lightNum's point of view
	//   (i.e., this is an image storing per-fragment surface normals for the
	//   back of the refractor)
	data->fbo->refract_NormalMap->BindBuffer();
		glClearDepth( frontSide );
		data->fbo->refract_NormalMap->ClearBuffers();
		SetLightMatrices( lightNum, data->fbo->refract_NormalMap->GetAspectRatio() );
		glDepthFunc( frontSide ? GL_LEQUAL : GL_GEQUAL );
		glClearDepth( 1.0 ); 
		glCullFace( GL_FRONT );
		data->shader->refract_DrawNormals->EnableShader();
		scene->DrawOnly( OBJECT_FLAGS_ISREFRACTIVE, MATL_FLAGS_NONE, OBJECT_OPTION_NONE, true );
		data->shader->refract_DrawNormals->DisableShader();
		glDepthFunc( GL_LEQUAL );
		glCullFace( GL_BACK );
		PopLightMatrices();
	data->fbo->refract_NormalMap->UnbindBuffer();

	// Draw an image with all opaque geometry in the scene.  I typically call this
	//    "background" geometry, though the code does not explicitly require it to 
	//    be *behind* the refractive object.
	data->fbo->caustic_BackgroundOBJs->BindBuffer();
		data->fbo->caustic_BackgroundOBJs->ClearBuffers();
		SetLightMatrices( lightNum, data->fbo->caustic_BackgroundOBJs->GetAspectRatio() );
		data->shader->refract_DrawNormals->EnableShader();
		scene->DrawOnly( OBJECT_FLAGS_ISBACKGROUND, MATL_FLAGS_NONE, OBJECT_OPTION_USE_LOWRES, true );
		data->shader->refract_DrawNormals->DisableShader();
		PopLightMatrices();
	data->fbo->caustic_BackgroundOBJs->UnbindBuffer();

	// Draw the photon buffer, from lightNum's point of view
	data->fbo->caustic_PhotonBuffer->BindBuffer();
		glClearColor( 1, 1, 1, -10 );
		data->fbo->caustic_PhotonBuffer->ClearBuffers();
		glClearColor( 0, 0, 0, 0 );
		SetLightMatrices( lightNum, data->fbo->caustic_PhotonBuffer->GetAspectRatio() );
		GLLight *light = scene->GetLight( lightNum );

		// Setup the shader (plus its parameters) that handles drawing the photon buffer
		data->shader->caustic_DrawPhotonBuf->EnableShader();
		data->shader->caustic_DrawPhotonBuf->BindAndEnableTexture( "backFace", data->fbo->refract_NormalMap->GetColorTextureID(), GL_TEXTURE1, GL_TEXTURE_2D );
		data->shader->caustic_DrawPhotonBuf->BindAndEnableTexture( "prevDistances", data->fbo->refract_NormalMap->GetDepthTextureID(), GL_TEXTURE2, GL_TEXTURE_2D );
		data->shader->caustic_DrawPhotonBuf->BindAndEnableTexture( "otherObjsEyeDepth", data->fbo->caustic_BackgroundOBJs->GetDepthTextureID(), GL_TEXTURE3, GL_TEXTURE_2D );
		float _near = light->GetLightNearPlane(), _far = light->GetLightFarPlane();
		data->shader->caustic_DrawPhotonBuf->SetParameter( "local1", _near*_far, _far-_near, _far+_near, _far ); 
		data->shader->caustic_DrawPhotonBuf->SetParameterv( "local2", 4, data->param->refract_ShaderLocal2 );

		// Actually draw the photon buffer.  We're only interested in photons that hit
		//   the refractive surface, so that's all we need to draw here.
		scene->DrawOnly( OBJECT_FLAGS_ISREFRACTIVE, MATL_FLAGS_NONE, OBJECT_OPTION_NONE, true );

		// Clean up.
		data->shader->caustic_DrawPhotonBuf->DisableTexture( GL_TEXTURE1 );
		data->shader->caustic_DrawPhotonBuf->DisableTexture( GL_TEXTURE2 );
		data->shader->caustic_DrawPhotonBuf->DisableTexture( GL_TEXTURE3 );
		data->shader->caustic_DrawPhotonBuf->DisableShader();
		PopLightMatrices();
	data->fbo->caustic_PhotonBuffer->UnbindBuffer();
	data->fbo->caustic_PhotonBuffer->AutomaticallyGenerateMipmaps( FBO_COLOR0 );

	// Create the caustic map using a multi-resolution "mipmap" approach
	FrameBuffer *drawFB = data->fbo->causticMapLayered;
	drawFB->BindBuffer();
		drawFB->ClearBuffers();
		SetLightMatrices( lightNum, drawFB->GetAspectRatio() );
		glDisable( GL_CULL_FACE );
		glDisable( GL_DEPTH_TEST );
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();
		HierarchialLightSpaceGather( drawFB->GetWidth() );
		glEnable( GL_CULL_FACE );
		glEnable( GL_DEPTH_TEST );
		PopLightMatrices();
	drawFB->UnbindBuffer(); 

	// Since we'll be using the multi-resolution / mipmapped caustic map, we
	//   either need to (a) change all the shaders that use it to look into
	//   multiple resolutions or (b) recombine the mipmap layers into a flat
	//   texture.  We choose to use (b), and this step performs the combination
	data->fbo->causticMap[lightNum]->BindBuffer();
		data->fbo->causticMap[lightNum]->ClearBuffers();
		CombineMipmapArrayLevels( data->fbo->causticMapLayered->GetColorTextureID() );
	data->fbo->causticMap[lightNum]->UnbindBuffer();

	// Tell the scene so it knows where the caustic map is stored.
	CopyDepthToCausticMap( data->fbo->causticMap[lightNum], data->fbo->caustic_BackgroundOBJs );
	scene->SetCausticMapID( lightNum, data->fbo->causticMap[lightNum]->GetColorTextureID() );
	scene->SetCausticDepthID( lightNum, data->fbo->causticMap[lightNum]->GetDepthTextureID() );
}




// Actually draw the scene with our hierarchical caustic maps
void Display_WithOriginalHierarchicalCaustics( void )
{
	float glassColor[4] = {1.0, 1.0, 1.0, 0.0};

	// Setup refraction data used multiple times for various shaders this frame
	data->param->refract_ShaderLocal2[0] = 1.0/(*data->param->indexOfRefraction); 
	data->param->refract_ShaderLocal2[1] = sqr( 1.0/(*data->param->indexOfRefraction) ); 
	data->param->refract_ShaderLocal2[2] = (*data->param->indexOfRefraction); 
	data->param->refract_ShaderLocal2[3] = sqr( *data->param->indexOfRefraction );

	// If the caustic map needs to be refreshed, regenerate it
	for (int i=0; i < scene->NumLightsEnabled(); i++)
		ComputeHierarchicalCausticMapForLightNumber( i ); 

	// Create shadow map(s)
	for (int i=0; i < scene->NumLightsEnabled(); i++)
		scene->CreateShadowMap( data->fbo->shadowMap[i], 
								data->param->shadowMatrixTrans[i], 
								i, // Light Number
								*(data->param->shadowMapBias) );

	// OK.  Now we've got our shadow maps and our caustic maps, so we need
	//     to actually draw the scene from the eye.  Below is this code.

	// First, render the buffers needed for deferred refraction from the 
	//     eye's point of view
	CreateRefractiveNormalMaps( 1 );
	CreateBackgroundTex( data->fbo->refract_BackgroundOBJs, true ); 

	// Now we're ready to begin actually drawing the final image from the eye's point of view
	data->fbo->mainWin->BindBuffer();
	data->fbo->mainWin->ClearBuffers();
	glLoadIdentity();
	scene->LookAtMatrix();
	scene->SetupEnabledLightsWithCurrentModelview();

	// First draw all the opaque, non refractive geometry.
	//    However, we already drew this in CreateBackgroundTex(), so instead of
	//    repeating this (possibly expensive) draw routine, we'll just copy the
	//    image and depth of the scene we obtained previously.
	CopyColorAndDepthTexturesIntoCurrentFBO( data->fbo->refract_BackgroundOBJs->GetColorTextureID(), 
											 data->fbo->refract_BackgroundOBJs->GetDepthTextureID() );

	// Now that we have the non-refractive stuff draw, draw our glass object.
	NonDeferredRefractionFromEyeView( glassColor );

	data->fbo->mainWin->UnbindBuffer();
}













