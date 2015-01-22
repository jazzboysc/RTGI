//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Texture2D.h"
#include "bmpread.h"
#include "Terminal.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Texture2D::Texture2D()
{
    mType = TT_Texture2D;
	Width = 0;
	Height = 0;
	LMax = 0.0f;
	RevGamma = 0.0f;
}
//----------------------------------------------------------------------------
Texture2D::~Texture2D()
{
}
//----------------------------------------------------------------------------
bool Texture2D::LoadBMPFromFile(GPUDevice* device, 
    const std::string& fileName, bool generateMipMap)
{
    if( mTextureHandle )
    {
        assert(false);
        return false;
    }

	IsRenderTarget = false;

	bmpread_t bitmap;
	if( !bmpread(fileName.c_str(), 0, &bitmap) )
	{
		Terminal::Output(Terminal::OC_Error, "Failed loading texture %s\n", 
            fileName.c_str());
        assert( false );
        
		return false;
	}
	Width = bitmap.width;
	Height = bitmap.height;

    // TODO:
    // Check this later.
    mInternalFormat = TIF_RGB8;
    mFormat = TF_RGB;
    mComponentType = TCT_Unsigned_Byte;

    mTextureHandle = GPU_DEVICE_FUNC(device, Texture2DLoadFromSystemMemory)(
        this, mInternalFormat, bitmap.width, bitmap.height, mFormat, 
        mComponentType, generateMipMap, bitmap.rgb_data);
    HasMipMap = generateMipMap;

    bmpread_free(&bitmap);
    
#ifdef RTGI_OUTPUT_TEXTURE_RESOURCE_LOADING
    Terminal::Output(Terminal::OC_Success, "Loading texture %s finished\n", 
        fileName.c_str());
#endif

	return true;
}
//----------------------------------------------------------------------------
bool Texture2D::LoadPFMFromFile(GPUDevice* device, 
    const std::string& fileName, bool generateMipMap)
{
	// This function is based on the PFM loader of Thorsten Grosch and Tobias 
	// Ritschel's demo.

    if( mTextureHandle )
    {
        assert(false);
        return false;
    }

	IsRenderTarget = false;
	IsHDRTexture = true;

    // Init some variables.
    char imageformat[1024]; 
    float f[1];

    std::string extendedFileName = "PLY/" + fileName;
	 
    // Open the file handle  .
    FILE* infile = fopen(extendedFileName.c_str(), "rb");
 
    if( infile == 0 )
	{ 
        Terminal::Output(Terminal::OC_Error, "Error loading %s !\n", 
            fileName.c_str());
		return false; 
    } 
 
    // Read the header.
    fscanf(infile, " %s %d %d ", (char*)&imageformat, &Width, &Height);
#ifdef RTGI_OUTPUT_TEXTURE_RESOURCE_LOADING
    Terminal::Output(Terminal::OC_Success, "Image format %s Width %d Height %d\n", 
        imageformat, Width, Height);
#endif
   
    float* pixels = (float*)(malloc(Width * Height * 3 * sizeof(float)));

    // Go ahead with the data.
    fscanf(infile, "%f", &f[0]); 
    fgetc(infile); 
 
    float red, green, blue;
    float *p = pixels; 
 
    // Read the values and store them.
    for( int j = 0; j < Height ; j++ )
	{ 
		for( int i = 0; i < Width ; i++ )
		{ 
			fread(f, 4, 1, infile); 
			red = f[0];
		     
			fread(f, 4, 1, infile); 
			green = f[0]; 
		     
			fread(f, 4, 1, infile); 
			blue = f[0]; 
		     
			*p++ = red;
			*p++ = green;
			*p++ = blue;
	 
			float L = (red + green + blue) / 3.0f; 
			if( L > LMax )
			{
			    LMax = L;
			}
		} 
    }

    mInternalFormat = TIF_RGB32F;
    mFormat = TF_RGB;
    mComponentType = TCT_Float;

    mTextureHandle = GPU_DEVICE_FUNC(device, Texture2DLoadFromSystemMemory)(
        this, mInternalFormat, Width, Height, mFormat, mComponentType, 
        generateMipMap, pixels);
    HasMipMap = generateMipMap;

	free(pixels);

#ifdef RTGI_OUTPUT_TEXTURE_RESOURCE_LOADING
    Terminal::Output(Terminal::OC_Success, "Loading texture %s finished\n", 
        fileName.c_str());
#endif
    
    RevGamma = 1.0f / 2.2f; 
	fclose(infile);

	return true;
}
//----------------------------------------------------------------------------
bool Texture2D::LoadFromSystemMemory(GPUDevice* device,
    TextureInternalFormat internalFormat, int width, int height,
    TextureFormat format, TextureComponentType type, void* pixels, 
    bool generateMipMap)
{
    if( mTextureHandle )
    {
        assert(false);
        return false;
    }

    Width = width;
    Height = height;
    mInternalFormat = internalFormat;
    mFormat = format;
    mComponentType = type;

    mTextureHandle = GPU_DEVICE_FUNC(device, Texture2DLoadFromSystemMemory)(
        this, mInternalFormat, Width, Height, mFormat, mComponentType, 
        generateMipMap, pixels);
    HasMipMap = generateMipMap;

	return true;
}
//----------------------------------------------------------------------------
#ifndef __APPLE__
bool Texture2D::LoadFromTextureBuffer(GPUDevice* device, 
    TextureBuffer* textureBuffer, TextureInternalFormat internalFormat)
{
    if( mTextureHandle )
    {
        return false;
    }

	IsTextureBuffer = true;
	mInternalFormat = internalFormat;

    mTextureHandle = GPU_DEVICE_FUNC(device, Texture2DLoadFromTextureBuffer)(
        this, textureBuffer, internalFormat);

	return true;
}
#endif
//----------------------------------------------------------------------------
void Texture2D::CreateRenderTarget(GPUDevice* device, int width, int height,
	TextureFormat format)
{
    if( mTextureHandle )
    {
        assert(false);
        return;
    }

	Width = width;
	Height = height;
	IsRenderTarget = true;
	mFormat = format;

	switch (mFormat)
	{
	case TF_RGB:
        mInternalFormat = TIF_RGB8;
        mComponentType = TCT_Unsigned_Byte;
		break;

    case TF_RGBA:
        mInternalFormat = TIF_RGBA8;
        mComponentType = TCT_Unsigned_Byte;
        break;

	case TF_RGBF:
		mInternalFormat = TIF_RGB32F;
        mComponentType = TCT_Float;
		break;

    case TF_RGBAF:
        mInternalFormat = TIF_RGBA32F;
        mComponentType = TCT_Float;
        break;

	case TF_R32UI:
#ifndef __APPLE__
		mInternalFormat = TIF_R32UI;
        mComponentType = TCT_Unsigned_Int;
#else
        assert( false );
#endif
		break;

    case TF_R32F:
        mInternalFormat = TIF_R32F;
        mComponentType = TCT_Float;
        break;

	case TF_Depth:
        mInternalFormat = TIF_Depth24;
        mComponentType = TCT_Unsigned_Byte;
		break;

	default:
		break;
	}

    mTextureHandle = GPU_DEVICE_FUNC(device, Texture2DLoadFromSystemMemory)(
        this, mInternalFormat, Width, Height, mFormat, mComponentType, 
        false, 0);
}
//--------------------------------------------------------------------------
void Texture2D::UpdateFromPixelBuffer(PixelBuffer* pixelBuffer)
{
    assert(mTextureHandle);
    GPU_DEVICE_FUNC(mTextureHandle->Device, 
        Texture2DUpdateFromPixelBuffer)(this, pixelBuffer);
}
//--------------------------------------------------------------------------
void Texture2D::CreateLDRandomTextureRGBF(GPUDevice* device, 
    int maxSampleCount, int patternSize)
{
	// This function is based on the PFM loader of Thorsten Grosch and 
	// Tobias Ritschel's demo.

    if( mTextureHandle )
    {
        assert(false);
        return;
    }

	int patternSizeSquared = patternSize * patternSize;

	srand(0);

	int haltonIndex = 0;
	float* const pixels = 
		new float[3 * maxSampleCount * patternSizeSquared];
	
	for( int i = 0; i < patternSizeSquared; i++ )
	{
		for( int j = 0; j < maxSampleCount; j++ )
		{
			glm::vec3 sample;
			do
			{
				sample = glm::vec3(2.0f * Halton(2, haltonIndex) - 1.0f, 
							  2.0f * Halton(3, haltonIndex) - 1.0f, 
							  Halton(5, haltonIndex));						
				haltonIndex++;
			} while( glm::length(sample) > 1.0f );

			pixels[(i * maxSampleCount + j) * 3 + 0] = sample[0];
			pixels[(i * maxSampleCount + j) * 3 + 1] = sample[1];
			pixels[(i * maxSampleCount + j) * 3 + 2] = sample[2];
		}
	}

    Width = maxSampleCount;
    Height = patternSizeSquared;
    mInternalFormat = TIF_RGB32F;
    mFormat = TF_RGB;
    mComponentType = TCT_Float;
	
    mTextureHandle = GPU_DEVICE_FUNC(device, Texture2DLoadFromSystemMemory)(
        this, mInternalFormat, Width, Height, mFormat, mComponentType,
        false, pixels);

	delete[] pixels;
}
//--------------------------------------------------------------------------
void Texture2D::GetImageData(void* dstPixels)
{
    assert(mTextureHandle);
    GPU_DEVICE_FUNC(mTextureHandle->Device, Texture2DGetImageData)(this, 
        dstPixels);
}
//--------------------------------------------------------------------------