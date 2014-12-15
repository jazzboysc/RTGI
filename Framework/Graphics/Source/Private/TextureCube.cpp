//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "TextureCube.h"
#include "bmpread.h"

using namespace RTGI;

//----------------------------------------------------------------------------
TextureCube::TextureCube()
{
}
//----------------------------------------------------------------------------
TextureCube::~TextureCube()
{
}
//----------------------------------------------------------------------------
bool TextureCube::LoadFromFile(const std::string& pX, const std::string& nX, 
	const std::string& pY, const std::string& nY, const std::string& pZ, 
	const std::string& nZ)
{
	bmpread_t pXBitmap, nXBitmap, pYBitmap, nYBitmap, pZBitmap, nZBitmap;
	int resPX = bmpread(pX.c_str(), 0, &pXBitmap);
	int resNX = bmpread(nX.c_str(), 0, &nXBitmap);
	int resPY = bmpread(pY.c_str(), 0, &pYBitmap);
	int resNY = bmpread(nY.c_str(), 0, &nYBitmap);
	int resPZ = bmpread(pZ.c_str(), 0, &pZBitmap);
	int resNZ = bmpread(nZ.c_str(), 0, &nZBitmap);
	if( !resPX || !resNX || !resPY || !resNY || !resPZ || !resNZ  )
	{
		assert( false );
		return false;
	}

    glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTexture);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, pXBitmap.width, 
		pXBitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, pXBitmap.rgb_data);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, nXBitmap.width, 
		nXBitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nXBitmap.rgb_data);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, pYBitmap.width, 
		pYBitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, pYBitmap.rgb_data);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, nYBitmap.width, 
		nYBitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nYBitmap.rgb_data);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, pZBitmap.width, 
		pZBitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, pZBitmap.rgb_data);

    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, nZBitmap.width, 
		nZBitmap.height, 0, GL_RGB, GL_UNSIGNED_BYTE, nZBitmap.rgb_data);

#ifdef _DEBUG
    GLenum res = glGetError();
    assert(res == GL_NO_ERROR);
#endif

	bmpread_free(&pXBitmap);
	bmpread_free(&nXBitmap);
	bmpread_free(&pYBitmap);
	bmpread_free(&nYBitmap);
	bmpread_free(&pZBitmap);
	bmpread_free(&nZBitmap);

	return true;
}
//----------------------------------------------------------------------------
Texture::TextureType TextureCube::GetType()
{
    return Texture::TT_TextureCube;
}
//--------------------------------------------------------------------------