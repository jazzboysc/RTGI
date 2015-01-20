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
    mType = TT_TextureCube;
    Width = 0;
    Height = 0;
}
//----------------------------------------------------------------------------
TextureCube::~TextureCube()
{
}
//----------------------------------------------------------------------------
bool TextureCube::LoadFromFile(GPUDevice* device, const std::string& pX, 
    const std::string& nX, const std::string& pY, const std::string& nY, 
    const std::string& pZ, const std::string& nZ)
{
    if( mTextureHandle )
    {
        assert(false);
        return false;
    }

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

    Width = pXBitmap.width;
    Height = pXBitmap.height;
    mFormat = TF_RGB;
    mInternalFormat = TIF_RGB8;
    mComponentType = TCT_Unsigned_Byte;

    mTextureHandle = GPU_DEVICE_FUNC(device, TextureCubeLoadFromSystemMemory)(
        this, mInternalFormat, Width, Height, mFormat, mComponentType,
        pXBitmap.rgb_data, nXBitmap.rgb_data, pYBitmap.rgb_data,
        nYBitmap.rgb_data, pZBitmap.rgb_data, nZBitmap.rgb_data);

	bmpread_free(&pXBitmap);
	bmpread_free(&nXBitmap);
	bmpread_free(&pYBitmap);
	bmpread_free(&nYBitmap);
	bmpread_free(&pZBitmap);
	bmpread_free(&nZBitmap);

	return true;
}
//----------------------------------------------------------------------------