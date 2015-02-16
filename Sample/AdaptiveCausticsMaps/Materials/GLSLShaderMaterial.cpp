/******************************************************************/
/* GLSLShaderMaterial.cpp                                         */
/* -----------------------                                        */
/*                                                                */
/* The file defines a complex OpenGL material defined by a GLSL   */
/*    shader.  This shader may consist of a vertex, geometry,     */
/*    and/or fragment shader.                                     */
/*                                                                */
/* Chris Wyman (02/01/2008)                                       */
/******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "Scene/Scene.h"
#include "GLSLShaderMaterial.h"
#include "Utils/ImageIO/imageIO.h"
#include "Utils/glslProgram.h"
#include "DataTypes/glTexture.h"

void GLSLShaderMaterial::SetupShadowMap( int lightNum, GLuint texID, float *matrix )
{
	char mapName[32];
	GLenum texUnit = GL_TEXTURE7-lightNum;
	sprintf( mapName, "shadowMap%d", lightNum );  // 7 = shadowMap0, 6 = shadowMap1, 5 = shadowMap2
	shader->BindAndEnableTexture( mapName, texID, texUnit, GL_TEXTURE_2D );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
	//shader->SetParameterv( "epS", 4, &matrix[0] );
	//shader->SetParameterv( "epT", 4, &matrix[4] );
	//shader->SetParameterv( "epR", 4, &matrix[8] );
	//shader->SetParameterv( "epQ", 4, &matrix[12] );
	glPushMatrix();
	glLoadIdentity();
	glTexGeni( GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
	glTexGeni( GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
	glTexGeni( GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
	glTexGeni( GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR );
	glTexGenfv( GL_S, GL_EYE_PLANE, &matrix[0] );
	glTexGenfv( GL_T, GL_EYE_PLANE, &matrix[4] );
	glTexGenfv( GL_R, GL_EYE_PLANE, &matrix[8] );
	glTexGenfv( GL_Q, GL_EYE_PLANE, &matrix[12] );
	glEnable( GL_TEXTURE_GEN_S );
	glEnable( GL_TEXTURE_GEN_T );
	glEnable( GL_TEXTURE_GEN_R );
	glEnable( GL_TEXTURE_GEN_Q );
	glPopMatrix();
	shader->SetParameter( "useShadowMap", 1 );
}

void GLSLShaderMaterial::DisableShadowMap( int lightNum )
{
	GLenum texUnit = GL_TEXTURE7-lightNum;
	glActiveTexture( texUnit );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
	glDisable( GL_TEXTURE_GEN_S );
	glDisable( GL_TEXTURE_GEN_T );
	glDisable( GL_TEXTURE_GEN_R );
	glDisable( GL_TEXTURE_GEN_Q );
	shader->DisableTexture( texUnit, GL_TEXTURE_2D );
}

void GLSLShaderMaterial::SetupCausticMap( int lightNum, GLuint causticID, GLuint causticDepthID )
{
	char mapName[32], depthName[32];
	sprintf( mapName, "causticMap%d", lightNum );  
	sprintf( depthName, "causticDepth%d", lightNum );  

	GLenum mapUnit = GL_TEXTURE5-lightNum;
	GLenum depthUnit = GL_TEXTURE3-lightNum;
	shader->BindAndEnableTexture( depthName, causticDepthID, depthUnit, GL_TEXTURE_2D );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE );
	shader->BindAndEnableTexture( mapName, causticID, mapUnit, GL_TEXTURE_2D );
	shader->SetParameter( "useCausticMap", 1 );
}

void GLSLShaderMaterial::DisableCausticMap( int lightNum )
{
	GLenum mapUnit = GL_TEXTURE5-lightNum;
	GLenum depthUnit = GL_TEXTURE3-lightNum;
	glActiveTexture( depthUnit );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE );
	shader->DisableTexture( depthUnit, GL_TEXTURE_2D );
	shader->DisableTexture( mapUnit, GL_TEXTURE_2D );
}



void GLSLShaderMaterial::Enable( Scene *s, unsigned int flags )
{
	if (!shader) return;
	inputScene = s;

	usingShadows  = (flags & MATL_FLAGS_USESHADOWMAP)  && ( SHADERMATL_ALLOWS_SHADOWMAPUSE & propertyFlags ); 
	usingCaustics = (flags & MATL_FLAGS_USECAUSTICMAP) && ( SHADERMATL_ALLOWS_CAUSTICMAPUSE & propertyFlags ); 

	shader->EnableShader();
	shader->SetParameter( "lightIntensity", s->GetLightIntensityModifier() );
	shader->SetParameter( "lightsEnabled", (float)s->NumLightsEnabled() );
	shader->SetParameterv( "sceneAmbient", 4, s->GetSceneAmbient() );
	if (usingShadows || usingCaustics)
	{
		for (int i=0; i< s->NumLightsEnabled(); i++)
			SetupShadowMap( i, s->GetShadowMapID( i ), s->GetShadowMapTransposeMatrix( i ) );
	}
	else
		shader->SetParameter( "useShadowMap", 0 );
	if (usingCaustics)
	{
		for (int i=0; i< s->NumLightsEnabled(); i++)
			SetupCausticMap( i, s->GetCausticMapID( i ), s->GetCausticDepthID( i ) );
	}
	else
		shader->SetParameter( "useCausticMap", 0 );
}

void GLSLShaderMaterial::Disable( void )
{
	if (!shader) return;

	if (usingShadows || usingCaustics) 
	{
		for (int i=0; i< inputScene->NumLightsEnabled(); i++)
			DisableShadowMap( i );
	}
	if (usingCaustics) 
	{	
		for (int i=0; i< inputScene->NumLightsEnabled(); i++)
			DisableCausticMap( i );
	}
	shader->DisableShader();
}


GLSLShaderMaterial::GLSLShaderMaterial( char *matlName ) : 
	Material( matlName ), shader(0), propertyFlags(SHADERMATL_NO_SPECIAL_BITS),
	vertFile(0), geomFile(0), fragFile(0)
{
	
}

GLSLShaderMaterial::~GLSLShaderMaterial()
{
	if (shader) delete shader;
	if (vertFile) free( vertFile );
	if (geomFile) free( geomFile );
	if (fragFile) free( fragFile );
	for (unsigned int i=0; i<bindNames.Size();i++)
		free( bindNames[i] );
}


GLSLShaderMaterial::GLSLShaderMaterial( FILE *f, Scene *s ) : 
	Material(), shader(0), propertyFlags(SHADERMATL_NO_SPECIAL_BITS),
	vertFile(0), geomFile(0), fragFile(0), geomSettingsUpdated(false),
	geomInputType(GL_TRIANGLES), geomOutputType(GL_TRIANGLE_STRIP),
	geomMaxEmittedVerts(0), enables(GLSL_NO_SPECIAL_STATE),
	disables(GLSL_NO_SPECIAL_STATE)
{
	bindTexNames.SetSize( 8 );
	bindTexs.SetSize( 8 );
	for (unsigned int i=0; i<bindTexNames.Size();i++)
	{
		bindTexNames[i] = 0;
		bindTexs[i] = 0;
	}

	// Search the scene file.
	char buf[ MAXLINELENGTH ], token[256], *ptr;
	while( fgets(buf, MAXLINELENGTH, f) != NULL )  
	{
		// Is this line a comment?
		ptr = StripLeadingWhiteSpace( buf );
		if (ptr[0] == '#') continue;

		// Nope.  So find out what the command is...
		ptr = StripLeadingTokenToBuffer( ptr, token );
		MakeLower( token );
	
		// Take different measures, depending on the command.
		if (!strcmp(token,"end")) break;
		if (!strcmp(token,"vertex") || !strcmp(token,"vert") || !strcmp(token,"v"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			vertFile = strdup( token );
		}
		else if (!strcmp(token,"geometry") || !strcmp(token,"geom") || !strcmp(token,"g"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			geomFile = strdup( token );
		}
		else if (!strcmp(token,"fragment") || !strcmp(token,"frag") || 
				 !strcmp(token,"f") || !strcmp(token,"pixel") || !strcmp(token,"p"))
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			fragFile = strdup( token );
		}
		else if (!strcmp(token,"isreflective") || !strcmp(token,"reflective"))  
			propertyFlags |= SHADERMATL_IS_REFLECTIVE;
		else if (!strcmp(token,"isrefractive") || !strcmp(token,"refractive")) 
			propertyFlags |= SHADERMATL_IS_REFRACTIVE;
		else if (!strcmp(token,"isglossy") || !strcmp(token,"glossy")) 
			propertyFlags |= SHADERMATL_IS_GLOSSY;
		else if (!strcmp(token,"allowsshadowmap") || !strcmp(token,"allowsshadows") || 
				 !strcmp(token,"shadows") || !strcmp(token,"shadow")) 
			propertyFlags |= SHADERMATL_ALLOWS_SHADOWMAPUSE;
		else if (!strcmp(token,"allowscausticmap") || !strcmp(token,"allowscaustics") || 
				 !strcmp(token,"caustics") || !strcmp(token,"allowcaustics")) 
			propertyFlags |= SHADERMATL_ALLOWS_CAUSTICMAPUSE;
		else if (!strcmp(token,"depth") || !strcmp(token,"depth_test") || !strcmp(token,"gl_depth_test") )
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if (!strcmp(token,"enable") || !strcmp(token,"enabled"))  enables |= GLSL_DEPTH_TEST;
			if (!strcmp(token,"disable")||!strcmp(token,"disabled"))  disables |= GLSL_DEPTH_TEST;
		}
		else if (!strcmp(token,"blend") || !strcmp(token,"gl_blend") )
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if (!strcmp(token,"enable") || !strcmp(token,"enabled"))  enables |= GLSL_BLEND;
			if (!strcmp(token,"disable")||!strcmp(token,"disabled"))  disables |= GLSL_BLEND;
		}
		else if (!strcmp(token,"stencil") || !strcmp(token,"gl_stencil_test") || !strcmp(token,"stencil_test") )
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if (!strcmp(token,"enable") || !strcmp(token,"enabled"))  enables |= GLSL_STENCIL_TEST;
			if (!strcmp(token,"disable")||!strcmp(token,"disabled"))  disables |= GLSL_STENCIL_TEST;
		}
		else if (!strcmp(token,"alpha") || !strcmp(token,"gl_alpha_test") || !strcmp(token,"alpha_test") )
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if (!strcmp(token,"enable") || !strcmp(token,"enabled"))  enables |= GLSL_ALPHA_TEST;
			if (!strcmp(token,"disable")||!strcmp(token,"disabled"))  disables |= GLSL_ALPHA_TEST;
		}
		else if (!strcmp(token,"culling") || !strcmp(token,"gl_cull_face") || !strcmp(token,"cull_face") )
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if (!strcmp(token,"enable") || !strcmp(token,"enabled"))  enables |= GLSL_CULL_FACE;
			if (!strcmp(token,"disable")||!strcmp(token,"disabled"))  disables |= GLSL_CULL_FACE;
		}
		else if (!strcmp(token,"lighting") || !strcmp(token,"gl_lighting") )
		{
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if (!strcmp(token,"enable") || !strcmp(token,"enabled"))  enables |= GLSL_LIGHTING;
			if (!strcmp(token,"disable")||!strcmp(token,"disabled"))  disables |= GLSL_LIGHTING;
		}
		else if (!strcmp(token,"bind"))
		{
			char shaderVarName[256];
			ptr = StripLeadingTokenToBuffer( ptr, shaderVarName );
			ptr = StripLeadingTokenToBuffer( ptr, token );
			MakeLower( token );
			if (!strcmp(token,"const"))
			{ // Bind a constant value
				bindConstNames.Add( strdup( shaderVarName ) );
				unsigned int idx = bindConstColors.Add( new Color( ptr ) );
			}
			else if (!strcmp(token, "tex"))
			{ // Bind a texture
				ptr = StripLeadingTokenToBuffer( ptr, token );
				int texID = atoi(token);
				if (texID < 0 || (unsigned int)texID > bindTexNames.Size())
					FatalError("Invalid texture ID number loading shader: %s", token);
				ptr = StripLeadingTokenToBuffer( ptr, token );
				bindTexNames[texID] = strdup( shaderVarName );
				GLTexture *tptr = s->GetNamedTexture( token );
				char *texFile=0;
				if (!tptr)
				{
					texFile = s->paths->GetTexturePath( token );
					tptr = s->ExistingTextureFromFile( texFile );
				}
				if (!tptr)
					s->AddTexture( tptr = 
						new GLTexture( texFile, 
									   TEXTURE_REPEAT_S | TEXTURE_REPEAT_T | TEXTURE_MIN_LINEAR_MIP_LINEAR, 
									   true ) );
				bindTexs[texID] = tptr;
				if (texFile) free( texFile );
			}
			else if (!strcmp(token, "vary"))
			{ // Bind a variable defined earlier in the scene file
				ptr = StripLeadingTokenToBuffer( ptr, token );
				MakeLower( token );
				float *fptr = s->GetSceneFloatVar( token );
				if (fptr)
				{
					bindNames.Add( strdup(shaderVarName) );
					bindPtrs.Add( fptr );
				}
				else Error("Unable to find scene variable named '%s'!", token);
			}

		}
		else
			Error("Unknown command '%s' when loading GLSLShaderMaterial!", token);
	}

	shaderPath = s->paths->GetShaderPathList();
}



void GLSLShaderMaterial::Preprocess( Scene *s )
{
	shader = new GLSLProgram( vertFile, geomFile, fragFile, true, shaderPath );
	shader->SetProgramEnables( enables );
	shader->SetProgramDisables( disables );
	if (geomSettingsUpdated) 
	{
		shader->GeometryShaderSettings( geomInputType, geomMaxEmittedVerts, geomOutputType );
		shader->LinkProgram();
	}
	for (unsigned int i=0; i<bindNames.Size(); i++)
		shader->SetupAutomaticBinding( bindNames[i], 1, bindPtrs[i] );
	for (unsigned int i=0; i<bindTexNames.Size(); i++)
		if (bindTexNames[i])
			shader->SetupAutomaticTextureBinding( bindTexNames[i], bindTexs[i]->TextureID(), GL_TEXTURE0+i, GL_TEXTURE_2D );
	for (unsigned int i=0; i<bindConstNames.Size(); i++)
		if (bindConstColors[i])
			shader->SetupAutomaticBinding( bindConstNames[i], 4, bindConstColors[i]->GetDataPtr() );

	s->AddShader( shader );
}


