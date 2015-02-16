/***************************************************************************/
/* adaptiveRefract.cpp                                                     */
/* -----------------------                                                 */
/*                                                                         */
/* Draw refractive objects using an adaptive method of photon tracing.     */
/*                                                                         */
/* Chris Wyman (04/25/2008)                                                */
/***************************************************************************/


#include "causticDemo.h"
#include "renderingData.h"
#include "renderingMethods.h"

extern Scene *scene;
extern RenderingData *data;

// Draws the opaque geometry seen behind the refractor into the
//   specified framebuffer object.  The withCaustics flag tells
//   our scene-renderer if it should apply an existing caustic map
//   to these surfaces.
void CreateBackgroundTex( FrameBuffer *fb, bool withCaustics )
{
	// Identify the background objects, from eye's point of view
	fb->BindBuffer();
		fb->ClearBuffers();
		glLoadIdentity();
		scene->LookAtMatrix();
		scene->SetupEnabledLightsWithCurrentModelview();
		unsigned int flags = withCaustics ? MATL_FLAGS_USECAUSTICMAP : 0;
		flags |= MATL_FLAGS_USESHADOWMAP;
		scene->DrawOnly( OBJECT_FLAGS_ISBACKGROUND, flags, OBJECT_OPTION_NONE );
	fb->UnbindBuffer();
}

// Draw the surface normals from the front and back side of a refractor
//   into a EXT_texture_array based framebuffer object.  The "drawWhat"
//   parameter is passed down to our render pipeline, allowing specification
//   of what geometry, exactly, should be drawn in this pass.  For simple
//   scenes, this is OBJECT_FLAGS_ISREFRACTIVE, but for the multi-layer
//   refraction shown in the paper, this function may need to be called 
//   multiple times, various flags to distinguish between the layers.  For
//   more robust applications, a flag won't work, and you'd actually need
//   to sort the geometry, at least coarsely, to figure out which refractive
//   object is in the back and should be drawn first.
void CreateRefractionBufs( unsigned int drawWhat )
{
	// Compute both front and back facing position and normal maps.
	data->fbo->deferredRefract_refractorGeom->BindBuffer();
		glClearColor(0, 0, 0, 1 );
		data->fbo->deferredRefract_refractorGeom->ClearBuffers();
		glLoadIdentity();
		scene->LookAtMatrix();
		glDisable( GL_CULL_FACE );
		data->shader->deferRefract_renderFrontAndBackNorms->EnableShader();
		scene->DrawOnly( drawWhat, MATL_FLAGS_NONE, OBJECT_OPTION_NONE, true );
		data->shader->deferRefract_renderFrontAndBackNorms->DisableShader();
		glEnable( GL_CULL_FACE );
	data->fbo->deferredRefract_refractorGeom->UnbindBuffer();
}

// Actually draw the refractive object with deferred refraction.
//    This is very similar to drawing a "standard" refractive object
//    (see code for that in "originalCausticMaps.cpp"), except 
//    instead of using a shader over fragments on the surface of the
//    refractor, we draw a screen-sized image and run the shader for
//    all fragments, either discarding them or computing the approximate
//    refraction location.
void DeferredRefraction( FrameBuffer *background, float *refractColor )
{
	// Again, this is a big mess, amounting to enabling the shader, setting
	//    the parameters it needs from the application, and then drawing
	//    a screen sized quad.
	GLSLProgram *refract = data->shader->deferredRefraction;
	refract->EnableShader();
	float idx = *data->param->indexOfRefraction;
	float _near = scene->GetCamera()->GetNear(), _far = scene->GetCamera()->GetFar();
	refract->SetParameter( "local1", _near*_far, _far-_near, _far+_near, _far); 
	refract->SetParameter( "local2", 1.0/idx, 1.0/(idx*idx), idx, idx*idx );
	refract->SetParameterv( "sceneAmbient", 4, scene->GetSceneAmbient() );
	refract->BindAndEnableTexture( "spotLight", data->tex->spotlight->TextureID(), GL_TEXTURE6, GL_TEXTURE_2D );
	refract->Set4x4MatrixParameterv( "gluOrtho",  data->param->gluOrtho );
	refract->SetParameter( "splatResolutionModifier", data->fbo->mainWin->GetWidth()/(float)data->fbo->deferredRefract_refractorGeom->GetWidth() );
	refract->Set4x4MatrixParameterv( "shadowMatrix", scene->GetShadowMapMatrix( 0 ) );	
	refract->SetParameter( "tanEyeFovy2", tan( M_PI*scene->GetCamera()->GetFovy()/360.0 ) );
	refract->BindAndEnableTexture( "geomNorm", data->fbo->deferredRefract_refractorGeom->GetColorTextureID( 0 ), GL_TEXTURE1, GL_TEXTURE_2D_ARRAY_EXT );
	refract->BindAndEnableTexture( "geomDepth", data->fbo->deferredRefract_refractorGeom->GetDepthTextureID(), GL_TEXTURE2, GL_TEXTURE_2D_ARRAY_EXT );
	refract->BindAndEnableTexture( "otherObjsEyeDepth", background->GetDepthTextureID(), GL_TEXTURE4, GL_TEXTURE_2D );
	refract->BindAndEnableTexture( "otherObjsEye", background->GetColorTextureID( 0 ), GL_TEXTURE5, GL_TEXTURE_2D );
	if (refractColor)
		refract->SetParameterv( "glassColor", 4, refractColor );

	// Draw the screen-size quad for deferred refraction
	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();
	glBegin( GL_QUADS );
		glTexCoord2f(0,0);	glVertex2f(0,0);
		glTexCoord2f(1,0);	glVertex2f(1,0);
		glTexCoord2f(1,1);	glVertex2f(1,1);
		glTexCoord2f(0,1);	glVertex2f(0,1);
	glEnd();
	
	// Clean up and disable the shader
	glPopMatrix();
	refract->DisableTexture( GL_TEXTURE1, GL_TEXTURE_2D_ARRAY_EXT );
	refract->DisableTexture( GL_TEXTURE2, GL_TEXTURE_2D_ARRAY_EXT );
	refract->DisableTexture( GL_TEXTURE4, GL_TEXTURE_2D );
	refract->DisableTexture( GL_TEXTURE5, GL_TEXTURE_2D );
	refract->DisableShader();
}


void Display_WithDeferredRefraction( void )
{
	float glassColor[4] = { 1.0, 1.0, 1.0, 0.0 };

	// Generate shadow map(s)...
	for (int i=0; i < scene->NumLightsEnabled(); i++)
		scene->CreateShadowMap( data->fbo->shadowMap[i], 
								data->param->shadowMatrixTrans[i], 
								i, // Light Number
								*(data->param->shadowMapBias) );

	// Create the buffers needed for refraction from the eye's point of view.
	CreateBackgroundTex( data->fbo->refract_BackgroundOBJs, false );
	CreateRefractionBufs( OBJECT_FLAGS_ISREFRACTIVE );

	// OK, draw the scene.  2 steps.  1) copy stuff seen behind the refractor, 2) draw the refractor
	data->fbo->mainWin->BindBuffer();
	data->fbo->mainWin->ClearBuffers();
	glLoadIdentity();
	scene->LookAtMatrix();
	scene->SetupEnabledLightsWithCurrentModelview();
	
	// Copy the background (created above) into the final buffer.
	CopyColorAndDepthTexturesIntoCurrentFBO( data->fbo->refract_BackgroundOBJs->GetColorTextureID(), 
											 data->fbo->refract_BackgroundOBJs->GetDepthTextureID() );

	// Draw the refractive object(s)
	DeferredRefraction( data->fbo->refract_BackgroundOBJs, glassColor );

	data->fbo->mainWin->UnbindBuffer();

}

