//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Texture1D_H
#define RTGI_Texture1D_H

#include "Texture.h"
#include "PixelBuffer.h"
#include "TextureBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/10/2014
//----------------------------------------------------------------------------
class Texture1D : public Texture
{
public:
	Texture1D();
	~Texture1D();

    bool LoadFromSystemMemory(GPUDevice* device, 
        BufferInternalFormat internalFormat, int width, BufferFormat format, 
        BufferComponentType type, void* pixels);

    // Update a render target from pixel buffer.
    void UpdateFromPixelBuffer(PixelBuffer* pixelBuffer);

    void CreateUniformRandomTexture(GPUDevice* device, int sampleCount, 
        int channelCount);

    void CreateHaltonRandomTexture(GPUDevice* device, int sampleCount,
        int channelCount);

    void GetDataFromGPUMemory(void* dstData);

	int Width;

protected:
    void SetupRandomTextureFormat(int sampleCount, int channelCount);
};

typedef RefPointer<Texture1D> Texture1DPtr;

}

#endif