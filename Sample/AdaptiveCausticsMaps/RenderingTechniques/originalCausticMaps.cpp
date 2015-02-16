/***************************************************************************/
/* simpleCausticMaps.cpp                                                   */
/* -----------------------                                                 */
/*                                                                         */
/* Draw the scene with refractive objects and simple caustics roughly      */
/*    equivalent to those proposed in the 2006 I3D paper by Wyman and      */
/*    Davis.                                                               */
/*                                                                         */
/* Chris Wyman (02/01/2008)                                                */
/***************************************************************************/

#include "causticDemo.h"
#include "renderingData.h"
#include "renderingMethods.h"

extern Scene *scene;
extern FrameRate *frameSpeed;
extern RenderingData *data;
extern FrameGrab *frameGrab;
            
// This is the same function (for objects behind the refractor)
//   used in the deferred shading version.
void CreateBackgroundTex( FrameBuffer *fb, bool withCaustics );

// Draws the normal maps (for the back of the geometry) for use in refraction
//   "frontSide" chooses if we'll use the closest of furthest back-facing surface
void CreateRefractiveNormalMaps( int frontSide )
{
	data->fbo->refract_NormalMap->BindBuffer();
	glClearDepth( (float)frontSide );
	data->fbo->refract_NormalMap->ClearBuffers();
	glLoadIdentity();
	scene->LookAtMatrix();

	glDepthFunc( frontSide ? GL_LEQUAL : GL_GEQUAL );
	glClearDepth( 1.0 );
	glCullFace( GL_FRONT );
	
	data->shader->refract_DrawNormals->EnableShader();
	scene->DrawOnly( OBJECT_FLAGS_ISREFRACTIVE, MATL_FLAGS_NONE, OBJECT_OPTION_NONE, true );
	data->shader->refract_DrawNormals->DisableShader();

	glDepthFunc( GL_LEQUAL );
	glCullFace( GL_BACK );
	data->fbo->refract_NormalMap->UnbindBuffer();
}

// Draws the normal maps (for the back of the geometry) for use in caustic refraction
//   "frontSide" chooses if we'll use the closest of furthest back-facing surface
void CreateCausticNormalMaps( int lightNum, int frontSide )
{
	data->fbo->refract_NormalMap->BindBuffer();
	glClearDepth( (float)frontSide );
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
}

// Draw the background objects for use in caustic computation.  This is a little
//    different than when we render refraction, since we don't need to store
//    *colors*, rather we need to store each fragment's *position*
void CreateCausticBackgroundTexture( int lightNum )
{
	data->fbo->caustic_BackgroundOBJs->BindBuffer();
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	data->fbo->caustic_BackgroundOBJs->ClearBuffers();
	SetLightMatrices( lightNum, data->fbo->caustic_BackgroundOBJs->GetAspectRatio() );

	data->shader->refract_DrawNormals->EnableShader();
	scene->DrawOnly( OBJECT_FLAGS_ISBACKGROUND, MATL_FLAGS_NONE, OBJECT_OPTION_USE_LOWRES, true );
	data->shader->refract_DrawNormals->DisableShader();
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	PopLightMatrices();
	data->fbo->caustic_BackgroundOBJs->UnbindBuffer();
}

// This computes the "photon buffer" (e.g., shown in Wyman & Davis 2006) based
//    upon two prior render passes (1 pass to store the background "opaque"
//    geometry, 1 pass to store the refractor's backfacing normal).
// These prior passes are computed above in CreateCausticNormalMaps()
//    and CreateCausticBackgroundTexture().
void DrawPhotonBuffer( int lightNum )
{	
	data->fbo->caustic_PhotonBuffer->BindBuffer();
	glClearColor( 1, 1, 1, -10 );
	data->fbo->caustic_PhotonBuffer->ClearBuffers();
	glClearColor( 0, 0, 0, 0 );
	SetLightMatrices( lightNum, data->fbo->caustic_PhotonBuffer->GetAspectRatio() );

	GLLight *light = scene->GetLight( lightNum );
	data->shader->caustic_DrawPhotonBuf->EnableShader();
	data->shader->caustic_DrawPhotonBuf->BindAndEnableTexture( "backFace", data->fbo->refract_NormalMap->GetColorTextureID(), GL_TEXTURE1, GL_TEXTURE_2D );
	data->shader->caustic_DrawPhotonBuf->BindAndEnableTexture( "prevDistances", data->fbo->refract_NormalMap->GetDepthTextureID(), GL_TEXTURE2, GL_TEXTURE_2D );
	data->shader->caustic_DrawPhotonBuf->BindAndEnableTexture( "otherObjsEyeDepth", data->fbo->caustic_BackgroundOBJs->GetDepthTextureID(), GL_TEXTURE3, GL_TEXTURE_2D );
	float _near = light->GetLightNearPlane(), _far = light->GetLightFarPlane();
	data->shader->caustic_DrawPhotonBuf->SetParameter( "local1", _near*_far, _far-_near, _far+_near, _far ); 
	data->shader->caustic_DrawPhotonBuf->SetParameterv( "local2", 4, data->param->refract_ShaderLocal2 );

	scene->DrawOnly( OBJECT_FLAGS_ISREFRACTIVE, MATL_FLAGS_NONE, OBJECT_OPTION_NONE, true );

	data->shader->caustic_DrawPhotonBuf->DisableTexture( GL_TEXTURE1 );
	data->shader->caustic_DrawPhotonBuf->DisableTexture( GL_TEXTURE2 );
	data->shader->caustic_DrawPhotonBuf->DisableTexture( GL_TEXTURE3 );
	data->shader->caustic_DrawPhotonBuf->DisableShader();

	PopLightMatrices();
	data->fbo->caustic_PhotonBuffer->UnbindBuffer();
} 

// This pass takes a photon buffer (computed above).  It runs through each
//    pixel in the buffer, treating the pixel as a photon position, and
//    splatting the resulting photon into the specificed caustic map
//    (using the viewing matrices for the specified light).
void CreateCausticMap( FrameBuffer *cMap, int lightNum )
{
	/* setup the texture we'll draw to */
	cMap->BindBuffer();
	cMap->ClearBuffers();
	SetLightMatrices( lightNum, cMap->GetAspectRatio() );

	/* set some necessary GL state. */
	glDisable( GL_CULL_FACE );
	glDisable( GL_DEPTH_TEST );

	/* the photon buffer vertex array already contains light-space coordinates!   */
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

	/* Draw the photons... */
	glBindBuffer( GL_ARRAY_BUFFER, data->vbo->genericTraversalBuffer[0] ); // Buffer containing garbage...  Needed b/c
	glVertexPointer( 2, GL_SHORT, 0, BUFFER_OFFSET( 0 ) );				   //   OpenGL *requires* vertex positions, even 
	glEnableClientState( GL_VERTEX_ARRAY );								   //   if we read positions from a texture.

	// Enable our shader, set up all the parameters we need to specify.
	data->shader->caustic_SimplePhotonSplat->EnableShader();
	data->shader->caustic_SimplePhotonSplat->BindAndEnableTexture( "photonPositions", 
					data->fbo->caustic_PhotonBuffer->GetColorTextureID(), GL_TEXTURE0, GL_TEXTURE_2D );
	data->shader->caustic_SimplePhotonSplat->BindAndEnableTexture( "lightColor", 
					data->tex->spotlight->TextureID(), GL_TEXTURE1, GL_TEXTURE_2D );
	data->shader->caustic_SimplePhotonSplat->SetParameter( "photonBufferResolution", 
					data->fbo->caustic_PhotonBuffer->GetWidth() );
	data->shader->caustic_SimplePhotonSplat->SetParameter( "causticMapResolution", 
					cMap->GetWidth() );
	data->shader->caustic_SimplePhotonSplat->SetParameter( "photonResolutionMultiplier", 
					sqr( cMap->GetWidth()/(float)data->fbo->caustic_PhotonBuffer->GetWidth() ) );

	// OK.  We enabled the shader, now start drawing.
	glEnable(GL_BLEND);                                        // We're drawing "splats" so enable blending
	glBlendFuncSeparate(GL_ONE, GL_ONE, GL_ONE, GL_ONE);       //   in order to correctly combine their 
	glBlendEquation(GL_FUNC_ADD);                              //   contributions.
	glPointSize( 3.0 );                                        // What size are the splats?	
	glDrawArraysInstancedEXT( GL_POINTS, 0,					   // Command to actually draw
							  data->fbo->caustic_PhotonBuffer->GetWidth(), 	
							  data->fbo->caustic_PhotonBuffer->GetWidth() ); 

	// Clean up point & blend state, disable shader
	glPointSize( 1.0 );                                        
    glDisable(GL_BLEND);                                       
	data->shader->caustic_SimplePhotonSplat->DisableTexture( GL_TEXTURE0, GL_TEXTURE_2D );
	data->shader->caustic_SimplePhotonSplat->DisableShader();

	// Stop rendering from a vertex array
    glDisableClientState( GL_VERTEX_ARRAY );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	// Disable other miscellaneous GL state & unbind the draw buffer
	glEnable( GL_CULL_FACE );
	glEnable( GL_DEPTH_TEST );
	PopLightMatrices();
	cMap->UnbindBuffer(); 
}

//  This function puts together the prior 4 functions, in order, and then 
//     sets flags in our rendering pipeline to use the correct caustic map
//     when rendering from the eye's point-of-view.
void ComputeCausticMapForLightNumber( int lightNum )
{
	// Create the photon buffer.
	CreateCausticNormalMaps( lightNum, 1 );     // For refraction
	CreateCausticBackgroundTexture( lightNum ); // To represent objects behind refractor
	DrawPhotonBuffer( lightNum );               // Use above buffers to find photon positions

	// Create the caustic map.
	CreateCausticMap( data->fbo->causticMap[lightNum], lightNum );  

	// Make sure that it has a depth buffer associated with it, so we can draw
	//    caustics only on opaque geometry *closest* to the light.  Luckily, we already have
	//    this buffer stored from the CreateCausticBackgroundTexture() pass, so we copy it
	//    instead of touching the geometry with another render pass.  If I was *really*
	//    intelligent, I could get a speed boost by avoiding this copy, but that would 
	//    probably require the background-texture buffer and caustic map to be the same 
	//    size, which I don't want in a prototype/research code.
	CopyDepthToCausticMap( data->fbo->causticMap[lightNum], data->fbo->caustic_BackgroundOBJs );

	// Set texture IDs so our scenegraph-like render structure can draw with caustic maps.
	scene->SetCausticMapID( lightNum, data->fbo->causticMap[lightNum]->GetColorTextureID() );
	scene->SetCausticDepthID( lightNum, data->fbo->causticMap[lightNum]->GetDepthTextureID() );
}

// This is very similar to the function DrawPhotonBuffer(), only it draws the
//    refractive object from the eye's point of view.  This is roughly functionally
//    equivalent to the function DeferredRefraction() in deferredRefract.cpp, though
//    the preliminaries are different.
void NonDeferredRefractionFromEyeView( float *glassColor )
{
	// First enable the refraction shader and its parameters
	float idx = *data->param->indexOfRefraction;
	float _near = scene->GetCamera()->GetNear(), _far = scene->GetCamera()->GetFar();
	data->shader->refract->EnableShader();
	data->shader->refract->BindAndEnableTexture( "backFace", data->fbo->refract_NormalMap->GetColorTextureID(), GL_TEXTURE1, GL_TEXTURE_2D );
	data->shader->refract->BindAndEnableTexture( "prevDistances", data->fbo->refract_NormalMap->GetDepthTextureID(), GL_TEXTURE2, GL_TEXTURE_2D );
	data->shader->refract->BindAndEnableTexture( "otherObjsEye", data->fbo->refract_BackgroundOBJs->GetColorTextureID(), GL_TEXTURE3, GL_TEXTURE_2D );
	data->shader->refract->BindAndEnableTexture( "otherObjsEyeDepth", data->fbo->refract_BackgroundOBJs->GetDepthTextureID(), GL_TEXTURE4, GL_TEXTURE_2D );
	data->shader->refract->BindAndEnableTexture( "spotLight", data->tex->spotlight->TextureID(), GL_TEXTURE5, GL_TEXTURE_2D );
	data->shader->refract->SetParameter( "useShadows", true );
	data->shader->refract->SetParameterv( "sceneAmbient", 4, scene->GetSceneAmbient() );
	data->shader->refract->SetParameter( "useFog", 0 );
	data->shader->refract->SetParameterv( "epS", 4, &data->param->shadowMatrixTrans[0][0] ); 
	data->shader->refract->SetParameterv( "epT", 4, &data->param->shadowMatrixTrans[0][4] ); 
	data->shader->refract->SetParameterv( "epR", 4, &data->param->shadowMatrixTrans[0][8] ); 
	data->shader->refract->SetParameterv( "epQ", 4, &data->param->shadowMatrixTrans[0][12] ); 
	data->shader->refract->SetParameter( "local1", _near*_far, _far-_near, _far+_near, _far); 
	data->shader->refract->SetParameter( "local2", 1.0/idx, 1.0/(idx*idx), idx, idx*idx );
	data->shader->refract->SetParameterv( "glassColor", 4, glassColor );

	// Draw the refractive object
	scene->DrawOnly( OBJECT_FLAGS_ISREFRACTIVE, MATL_FLAGS_NONE, OBJECT_OPTION_NONE, true );

	// Disable the shader / clean up.
	data->shader->refract->DisableTexture( GL_TEXTURE1 );
	data->shader->refract->DisableTexture( GL_TEXTURE2 );
	data->shader->refract->DisableTexture( GL_TEXTURE3 );
	data->shader->refract->DisableTexture( GL_TEXTURE4 );
	data->shader->refract->DisableTexture( GL_TEXTURE5 );
	data->shader->refract->DisableShader();

}


void Display_WithOriginalRefractiveCaustics( void )
{
	float glassColor[4] = {1.0, 1.0, 1.0, 0.0};

	// Setup refraction data used multiple times for various shaders this frame
	data->param->refract_ShaderLocal2[0] = 1.0/(*data->param->indexOfRefraction); 
	data->param->refract_ShaderLocal2[1] = 1.0/((*data->param->indexOfRefraction)*(*data->param->indexOfRefraction)); 
	data->param->refract_ShaderLocal2[2] = (*data->param->indexOfRefraction); 
	data->param->refract_ShaderLocal2[3] = (*data->param->indexOfRefraction)*(*data->param->indexOfRefraction);

	// If the caustic map needs to be refreshed, regenerate it
	for (int i=0; i < scene->NumLightsEnabled(); i++)
		ComputeCausticMapForLightNumber( i ); 

	// Generate shadow map(s)...
	for (int i=0; i < scene->NumLightsEnabled(); i++)
		scene->CreateShadowMap( data->fbo->shadowMap[i], 
								data->param->shadowMatrixTrans[i], 
								i, // Light Number
								*(data->param->shadowMapBias) );

	// Create the buffers needed for refraction from the eye's point of view.
	CreateRefractiveNormalMaps( 1 );
	CreateBackgroundTex( data->fbo->refract_BackgroundOBJs, true ); 

	data->fbo->mainWin->BindBuffer();
	data->fbo->mainWin->ClearBuffers();
	glLoadIdentity();
	scene->LookAtMatrix();
	scene->SetupEnabledLightsWithCurrentModelview();

	// Copy the background (created above) into the final buffer.
	CopyColorAndDepthTexturesIntoCurrentFBO( data->fbo->refract_BackgroundOBJs->GetColorTextureID(), 
											 data->fbo->refract_BackgroundOBJs->GetDepthTextureID() );

	// Now that we have the non-refractive stuff draw, draw our glass object.
	NonDeferredRefractionFromEyeView( glassColor );

	data->fbo->mainWin->UnbindBuffer();
}





