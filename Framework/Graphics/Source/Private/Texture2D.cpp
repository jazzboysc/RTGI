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
bool Texture2D::LoadBMPFromFile(const std::string& fileName)
{
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

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bitmap.width, bitmap.height, 0, 
		GL_RGB, GL_UNSIGNED_BYTE, bitmap.rgb_data);
	bmpread_free(&bitmap);

	mInternalFormat = GL_RGB;
	mFormat = GL_RGB;
	mType = GL_UNSIGNED_BYTE;
    
#ifdef RTGI_OUTPUT_TEXTURE_RESOURCE_LOADING
    Terminal::Output(Terminal::OC_Success, "Loading texture %s finished\n", 
        fileName.c_str());
#endif

	return true;
}
//----------------------------------------------------------------------------
bool Texture2D::LoadPFMFromFile(const std::string& fileName)
{
	// This function is based on the PFM loader of Thorsten Grosch and Tobias 
	// Ritschel's demo.

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

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, Width, Height, 0, GL_RGB,
		GL_FLOAT, pixels);

	mInternalFormat = GL_RGB32F_ARB;
	mFormat = GL_RGB;
	mType = GL_FLOAT;

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
bool Texture2D::LoadFromSystemMemory(GLint internalFormat, GLsizei width, 
	GLsizei height, GLenum format, GLenum type, void* pixels)
{
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, 
		type, pixels);

	Width = width;
	Height = height;
	mInternalFormat = internalFormat;
	mFormat = format;
	mType = type;

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}
//----------------------------------------------------------------------------
#ifndef __APPLE__
bool Texture2D::LoadFromTextureBuffer(TextureBuffer* textureBuffer, 
	GLenum internalFormat)
{
	IsTextureBuffer = true;
	mInternalFormat = internalFormat;

	GLuint buffer = textureBuffer->GetBuffer();
    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_BUFFER, mTexture);
    glTexBuffer(GL_TEXTURE_BUFFER, internalFormat, buffer);
    glBindTexture(GL_TEXTURE_BUFFER, 0);

	return true;
}
#endif
//----------------------------------------------------------------------------
void Texture2D::CreateRenderTarget(int width, int height, 
	TextureFormat format)
{
	Width = width;
	Height = height;
	IsRenderTarget = true;
	RTFormat = format;

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);	

	switch (RTFormat)
	{
	case RTGI::Texture2D::TF_RGB:
        mInternalFormat = GL_RGB8;
        mFormat = GL_RGB;
        mType = GL_UNSIGNED_BYTE;
		break;

    case RTGI::Texture2D::TF_RGBA:
        mInternalFormat = GL_RGBA8;
        mFormat = GL_RGBA;
        mType = GL_UNSIGNED_BYTE;
        break;

	case RTGI::Texture2D::TF_RGBF:
		mInternalFormat = GL_RGB32F_ARB;
		mFormat = GL_RGB;
		mType = GL_FLOAT;
		break;

    case RTGI::Texture2D::TF_RGBAF:
        mInternalFormat = GL_RGBA32F_ARB;
        mFormat = GL_RGBA;
        mType = GL_FLOAT;
        break;

	case RTGI::Texture2D::TF_R32UI:
#ifndef __APPLE__
		mInternalFormat = GL_R32UI;
		mFormat = GL_RED_INTEGER;
		mType = GL_UNSIGNED_INT;
#else
        assert( false );
#endif
		break;

    case RTGI::Texture2D::TF_R32F:
        mInternalFormat = GL_R32F;
        mFormat = GL_RED;
        mType = GL_FLOAT;
        break;

	case RTGI::Texture2D::TF_Depth:
		mInternalFormat = GL_DEPTH_COMPONENT24;
		mFormat = GL_DEPTH_COMPONENT;
		mType = GL_UNSIGNED_BYTE;
		break;

	default:
		break;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, width, height, 0, 
		mFormat, mType, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBindTexture(GL_TEXTURE_2D, 0);	
}
//--------------------------------------------------------------------------
void Texture2D::UpdateFromPixelBuffer(PixelBuffer* pixelBuffer)
{
	GLuint buffer = pixelBuffer->GetBuffer();
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, mInternalFormat, Width, Height, 0, 
		mFormat, mType, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}
//--------------------------------------------------------------------------
void Texture2D::BindToImageUnit(GLuint unit, GLenum access)
{
#if defined(__APPLE__)
    assert( false );
#else
	glBindImageTexture(unit, mTexture, 0, GL_FALSE, 0, access, 
		mInternalFormat);
#endif
}
//--------------------------------------------------------------------------
void Texture2D::CreateLDRandomTextureRGBF(int maxSampleCount, 
    int patternSize)
{
	// This function is based on the PFM loader of Thorsten Grosch and 
	// Tobias Ritschel's demo.

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

    mInternalFormat = GL_RGB32F_ARB;
    mFormat = GL_RGB;
    mType = GL_FLOAT;
	
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, maxSampleCount, 
		patternSizeSquared, 0, GL_RGB, GL_FLOAT, pixels); 
			
	delete[] pixels;
}
//--------------------------------------------------------------------------
void Texture2D::GetImageData(void* dstPixels)
{
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glGetTexImage(GL_TEXTURE_2D, 0, mFormat, mType, dstPixels);
    glBindTexture(GL_TEXTURE_2D, 0);
}
//--------------------------------------------------------------------------
Texture::TextureType Texture2D::GetType()
{
    return Texture::TT_Texture2D;
}
//--------------------------------------------------------------------------