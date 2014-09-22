//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_TextureCube_H
#define RTGI_TextureCube_H

#include "FrameworkCommon.h"
#include "Texture.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/07/2013
//----------------------------------------------------------------------------
class TextureCube : public Texture
{
public:
	TextureCube();
	~TextureCube();

	// Load six ".bmp" files and create a cube-map resource.
	bool LoadFromFile(const std::string& pX, const std::string& nX, 
		const std::string& pY, const std::string& nY, const std::string& pZ, 
		const std::string& nZ);

    virtual TextureType GetType();
};

typedef RefPointer<TextureCube> TextureCubePtr;

}

#endif