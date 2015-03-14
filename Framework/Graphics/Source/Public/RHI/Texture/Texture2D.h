//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Texture2D_H
#define RTGI_Texture2D_H

#include "Texture.h"
#include "PixelBuffer.h"
#include "TextureBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 11/07/2013
//----------------------------------------------------------------------------
class Texture2D : public Texture
{
public:
	Texture2D();
	~Texture2D();

	// Load a ".bmp" file and create its resource.
    bool LoadBMPFromFile(GPUDevice* device, const std::string& fileName, 
        bool generateMipMap = true);

	// Load a ".pfm" file and create its resource.
    bool LoadPFMFromFile(GPUDevice* device, const std::string& fileName,
        bool generateMipMap = true);

	// Load texture data from system memory. User is responsible for deleting
	// the system memory data.
    bool LoadFromSystemMemory(GPUDevice* device, 
        BufferInternalFormat internalFormat, int width, int height, 
        BufferFormat format, BufferComponentType type, void* pixels,
        bool generateMipMap = true);

#ifndef __APPLE__
	// Load and create from texture buffer object.
    bool LoadFromTextureBuffer(GPUDevice* device, TextureBuffer* textureBuffer,
        BufferInternalFormat internalFormat);
#endif

    void CreateRenderTarget(GPUDevice* device, int width, int height, 
        BufferFormat format, bool generateMipMap = false, void* initData = 0);

	// Update a render target from pixel buffer.
	void UpdateFromPixelBuffer(PixelBuffer* pixelBuffer);

	// Generate random positions inside a unit hemisphere based on halton 
	// numbers.
    void CreateLDRandomTextureRGBF(GPUDevice* device, int maxSampleCount, 
        int patternSize);

    void GetImageData(void* dstPixels);

	int Width, Height;
	float LMax;
	float RevGamma;
};

typedef RefPointer<Texture2D> Texture2DPtr;

}

#endif