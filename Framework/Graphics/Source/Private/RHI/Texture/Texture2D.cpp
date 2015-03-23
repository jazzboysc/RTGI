//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#include "Texture2D.h"
#include "bmpread.h"
#include "png.h"
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
    mInternalFormat = BIF_RGB8;
    mFormat = BF_RGB;
    mComponentType = BCT_Unsigned_Byte;

    mTextureHandle = device->Texture2DLoadFromSystemMemory(
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
bool Texture2D::LoadPNGFromFile(GPUDevice* device, 
    const std::string& fileName, bool generateMipMap)
{
    if( mTextureHandle )
    {
        assert(false);
        return false;
    }

    IsRenderTarget = false;

    png_image png;
    memset(&png, 0, sizeof(png_image));
    png.version = PNG_IMAGE_VERSION;

    if( png_image_begin_read_from_file(&png, fileName.c_str()) )
    {
        png_bytep buffer;

        png.format = PNG_FORMAT_RGBA;
        buffer = (png_bytep)malloc(PNG_IMAGE_SIZE(png));

        if( png_image_finish_read(&png, 0, buffer, 0, 0) )
        {
            Width = png.width;
            Height = png.height;

            mInternalFormat = BIF_RGBA8;
            mFormat = BF_RGBA;
            mComponentType = BCT_Unsigned_Byte;

            mTextureHandle = device->Texture2DLoadFromSystemMemory(
                this, mInternalFormat, Width, Height, mFormat,
                mComponentType, generateMipMap, buffer);
            HasMipMap = generateMipMap;

#ifdef RTGI_OUTPUT_TEXTURE_RESOURCE_LOADING
            Terminal::Output(Terminal::OC_Success, 
                "Loading texture %s finished\n", fileName.c_str());
#endif
            free(buffer);

            return true;
        }
        else
        {
            free(buffer);
        }
    }

    return false;
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

    mInternalFormat = BIF_RGB32F;
    mFormat = BF_RGB;
    mComponentType = BCT_Float;

    mTextureHandle = device->Texture2DLoadFromSystemMemory(
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
    BufferInternalFormat internalFormat, int width, int height,
    BufferFormat format, BufferComponentType type, void* pixels, 
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

    mTextureHandle = device->Texture2DLoadFromSystemMemory(
        this, mInternalFormat, Width, Height, mFormat, mComponentType, 
        generateMipMap, pixels);
    HasMipMap = generateMipMap;

	return true;
}
//----------------------------------------------------------------------------
#ifndef __APPLE__
bool Texture2D::LoadFromTextureBuffer(GPUDevice* device, 
    TextureBuffer* textureBuffer, BufferInternalFormat internalFormat)
{
    if( mTextureHandle )
    {
        return false;
    }

	IsTextureBuffer = true;
	mInternalFormat = internalFormat;

    mTextureHandle = device->Texture2DLoadFromTextureBuffer(
        this, textureBuffer, internalFormat);

	return true;
}
#endif
//----------------------------------------------------------------------------
void Texture2D::CreateRenderTarget(GPUDevice* device, int width, int height,
    BufferFormat format, bool generateMipMap, void* initData)
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
	case BF_RGB:
        mInternalFormat = BIF_RGB8;
        mComponentType = BCT_Unsigned_Byte;
		break;

    case BF_RGBA:
        mInternalFormat = BIF_RGBA8;
        mComponentType = BCT_Unsigned_Byte;
        break;

	case BF_RGBF:
		mInternalFormat = BIF_RGB32F;
        mComponentType = BCT_Float;
		break;

    case BF_RGBAF:
        mInternalFormat = BIF_RGBA32F;
        mComponentType = BCT_Float;
        break;

	case BF_R32UI:
#ifndef __APPLE__
		mInternalFormat = BIF_R32UI;
        mComponentType = BCT_Unsigned_Int;
#else
        assert( false );
#endif
		break;

    case BF_R32F:
        mInternalFormat = BIF_R32F;
        mComponentType = BCT_Float;
        break;

	case BF_Depth:
        mInternalFormat = BIF_Depth24;
        mComponentType = BCT_Unsigned_Byte;
		break;

	default:
		break;
	}

    mTextureHandle = device->Texture2DLoadFromSystemMemory(
        this, mInternalFormat, Width, Height, mFormat, mComponentType, 
        generateMipMap, initData);
    HasMipMap = generateMipMap;
}
//--------------------------------------------------------------------------
void Texture2D::UpdateFromPixelBuffer(PixelBuffer* pixelBuffer)
{
    assert(mTextureHandle);
    mTextureHandle->Device->Texture2DUpdateFromPixelBuffer(this, pixelBuffer);
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

	int haltonIndex = 1;
	float* const pixels = 
		new float[3 * maxSampleCount * patternSizeSquared];
	
	for( int i = 0; i < patternSizeSquared; i++ )
	{
		for( int j = 0; j < maxSampleCount; j++ )
		{
			glm::vec3 sample;
			do
			{
				sample = glm::vec3(2.0f * RadicalInverse(2, haltonIndex) - 1.0f, 
							  2.0f * RadicalInverse(3, haltonIndex) - 1.0f, 
							  RadicalInverse(5, haltonIndex));						
				haltonIndex++;
			} while( glm::length(sample) > 1.0f );

			pixels[(i * maxSampleCount + j) * 3 + 0] = sample[0];
			pixels[(i * maxSampleCount + j) * 3 + 1] = sample[1];
			pixels[(i * maxSampleCount + j) * 3 + 2] = sample[2];
		}
	}

    Width = maxSampleCount;
    Height = patternSizeSquared;
    mInternalFormat = BIF_RGB32F;
    mFormat = BF_RGB;
    mComponentType = BCT_Float;
	
    mTextureHandle = device->Texture2DLoadFromSystemMemory(
        this, mInternalFormat, Width, Height, mFormat, mComponentType,
        false, pixels);

	delete[] pixels;
}
//--------------------------------------------------------------------------
void Texture2D::GetImageData(void* dstPixels)
{
    assert(mTextureHandle);
    mTextureHandle->Device->Texture2DGetImageData(this, 
        dstPixels);
}
//--------------------------------------------------------------------------