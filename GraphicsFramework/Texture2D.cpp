//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------
#include "GraphicsFrameworkPCH.h"
#include "Texture2D.h"
#include "bmpread.h"
#include "vec.h"

using namespace RTGI;

//----------------------------------------------------------------------------
Texture2D::Texture2D()
{
	Width = 0;
	Height = 0;
	IsRenderTarget = false;
	IsHDRTexture = false;
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
		printf("Failed loading texture %s\n", fileName.c_str());
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
    
#ifdef RTGI_OUTPUT_RESOURCE_LOADING
    printf("Loading texture %s finished\n", fileName.c_str());
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
	 
    // Open the file handle  .
	FILE* infile = fopen(fileName.c_str(), "rb"); 
 
    if( infile == 0 )
	{ 
		printf("Error loading %s !\n", fileName.c_str()); 
		return false; 
    } 
 
    // Read the header.
    fscanf(infile, " %s %d %d ", (char*)&imageformat, &Width, &Height);
#ifdef RTGI_OUTPUT_RESOURCE_LOADING
    printf("Image format %s Width %d Height %d\n", imageformat, Width, Height);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, Width, Height, 0, GL_RGB, 
		GL_FLOAT, pixels);

	free(pixels);

#ifdef RTGI_OUTPUT_RESOURCE_LOADING
    printf("Loading texture %s finished\n", fileName.c_str());
#endif
    
    RevGamma = 1.0f / 2.2f; 
	fclose(infile);

	return true;
}
//----------------------------------------------------------------------------
bool Texture2D::LoadFromSystemMemory(GLint internalFormat, GLsizei width, 
	GLsizei height, GLenum format, GLenum type, void* pixels)
{
	assert( pixels );

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, 
		type, pixels);

	return true;
}
//----------------------------------------------------------------------------
void Texture2D::CreateRenderTarget(int width, int height, 
	RenderTargetFormat format)
{
	Width = width;
	Height = height;
	IsRenderTarget = true;
	RTFormat = format;

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);	

	switch (RTFormat)
	{
	case RTGI::Texture2D::RTF_RGB:
		// TODO:
		assert( false );
		break;

	case RTGI::Texture2D::RTF_RGBF:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, width, height, 0, GL_RGB, 
			GL_FLOAT, 0);
		break;

	case RTGI::Texture2D::RTF_Depth:
		glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, 
			GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		break;

	default:
		break;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
//--------------------------------------------------------------------------
void Texture2D::CreateLDRandomTexture(int maxSampleCount, int patternSize)
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
			vec3 sample;
			do
			{
				sample = vec3(2.0f * Halton(2, haltonIndex) - 1.0f, 
							  2.0f * Halton(3, haltonIndex) - 1.0f, 
							  Halton(5, haltonIndex));						
				haltonIndex++;
			} while( length(sample) > 1.0f );

			pixels[(i * maxSampleCount + j) * 3 + 0] = sample[0];
			pixels[(i * maxSampleCount + j) * 3 + 1] = sample[1];
			pixels[(i * maxSampleCount + j) * 3 + 2] = sample[2];
		}
	}
	
	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, maxSampleCount, 
		patternSizeSquared, 0, GL_RGB, GL_FLOAT, pixels); 
			
	delete pixels;
}
//--------------------------------------------------------------------------