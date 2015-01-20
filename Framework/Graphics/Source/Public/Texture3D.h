//----------------------------------------------------------------------------
// Graphics framework for real-time GI study.
// Che Sun at Worcester Polytechnic Institute, Fall 2013.
//----------------------------------------------------------------------------

#ifndef RTGI_Texture3D_H
#define RTGI_Texture3D_H

#include "Texture.h"
#include "PixelBuffer.h"
#include "TextureBuffer.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 08/05/2014
//----------------------------------------------------------------------------
class Texture3D : public Texture
{
public:
	Texture3D();
	~Texture3D();

    bool LoadFromSystemMemory(GPUDevice* device, 
        TextureInternalFormat internalFormat, int width, int height, int depth, 
        TextureFormat format, TextureComponentType type, void* pixels);

    void CreateRenderTarget(GPUDevice* device, int width, int height, 
        int depth, TextureFormat format);

		// Update a render target from pixel buffer.
	void UpdateFromPixelBuffer(PixelBuffer* pixelBuffer);

	int Width, Height, Depth;
};

typedef RefPointer<Texture3D> Texture3DPtr;

}

#endif