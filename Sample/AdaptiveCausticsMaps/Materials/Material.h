/******************************************************************/
/* Material.h                                                     */
/* -----------------------                                        */
/*                                                                */
/* The file defines a base material type that is inherited by     */
/*     more complex material types.                               */
/*                                                                */
/* Chris Wyman (02/01/2008)                                       */
/******************************************************************/

#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

class GLTexture;
class GLSLProgram;
class Scene;

// These flags may or may not be accepted by all material types...
//   MATL_FLAGS_NONE says "use default material parameters", other
//   flags may enabled/disable abilities depending on the material.
#define MATL_FLAGS_NONE					0x00000000
#define MATL_FLAGS_USESHADOWMAP			0x00000001
#define MATL_FLAGS_USECAUSTICMAP		0x00000002



#pragma warning( disable: 4996 )

class Material
{
private:
	char *name;

public:
	// A constructor for the base material class
	Material( char *matlName="<Unnamed Material>" )     { if (matlName) name = strdup( matlName ); else name=0; }
	Material( FILE *f, Scene *s ) 
	    { printf("Constructor Material::Material( FILE *f ) called.  This is not implemented!"); }
	~Material()            { if (name) free(name); }

	// All materials must be able to be Enable()'d or Disable()'d
	virtual void Enable( Scene *s, unsigned int flags=MATL_FLAGS_NONE )  = 0;
	virtual void Disable( void ) = 0;

	// Some materials may need preprocess (e.g., shaders which need GL initialized)
	virtual bool NeedsPreprocessing( void )				{ return false; }
	virtual void Preprocess( Scene * )                  { }

	// Get or set the name of the material
	inline char *GetName( void )                 { return name; }
	inline void SetName( const char *newName )   { if (name) free (name);  name = strdup(newName); }

	// There are a number of questions one might wish to know about a material
	//   Feel free to add more, but make sure there is a default method, do not
	//   have any abstract methods in this category!
	virtual bool UsesTexture( void )					{ return false; }
	virtual bool UsesAlpha( void )						{ return false; }
	virtual bool UsesLighting( void )					{ return false; }
	virtual bool UsesShaders( void )					{ return false; }
	virtual bool HandlesShadowMap( void )				{ return false; }

	// Perhaps some sorts of materials need to be handled specially...
	virtual bool IsReflective( void )                   { return false; }
	virtual bool IsRefractive( void )                   { return false; }
	virtual bool IsGlossy( void )						{ return false; }


	// Some of the above queries naturally have follow-up queries (e.g., if the
	//   shader uses a texture, what *is* the texture?).  The following methods
	//   allow access to this data.  Again, no abstract methods.  Probably, these
	//   will all return pointers, which should default to NULL if no such thing
	//   exists (or the material doesn't want to give up control of the resource)
	virtual GLTexture *GetMaterialTexture( void )		{ return NULL; }
	virtual GLSLProgram *GetMaterialShader( void )	{ return NULL; }

};




#endif

