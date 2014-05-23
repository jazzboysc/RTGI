//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Texture_H
#define RTGI_Texture_H

#include "FrameworkCommon.h"
#include "RefObject.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/07/2013
//----------------------------------------------------------------------------
class Texture : public RefObject
{
public:
	Texture();
	virtual ~Texture();

	GLuint GetTexture() const;

protected:
	GLuint mTexture;
};

typedef RefPointer<Texture> TexturePtr;

}

#endif