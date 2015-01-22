#ifndef RTGI_OITApp_H
#define RTGI_OITApp_H

#include "GraphicsFrameworkHeader.h"
#include "OITTriMesh.h"
#include "OITScreenQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 07/22/2014
//----------------------------------------------------------------------------
class OITApp : public Application
{
public:
	OITApp(int width = 768, int height = 768);
	~OITApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;

private:
	AtomicCounterBufferPtr mGPUMemAllocCounter;
	Texture2DPtr mHeadPointerTexture;
	PixelBufferPtr mHeadPointerTextureInitData;
	Texture2DPtr mGPUMemPoolTexture;
	TextureBufferPtr mGPUMemPool;
	StructuredBufferPtr mGPUMemPool2;
    
	OITScreenQuadPtr mScreenQuad;
    OITTriMeshPtr mModel;
};

}

#endif