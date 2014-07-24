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
	OITApp(int width, int height);
	~OITApp();

	void Initialize();
	void Run();
	void Terminate();

	// Event handlers.
	void OnKeyboard(unsigned char key, int x, int y);
	void OnKeyboardUp(unsigned char key, int x, int y);
	void OnMouse(int button, int state, int x, int y);
	void OnMouseMove(int x, int y);
	void OnReshape(int x, int y);

private:
	int mWidth, mHeight;
	std::string mWindowTitle;

	Camera* mCamera;

	Texture2DPtr mHeadPointerTexture;
	PixelBufferPtr mHeadPointerTextureInitData;
	AtomicCounterBufferPtr mGPUMemAllocCounter;
	TextureBufferPtr mGPUMemPool;
    
	OITScreenQuadPtr mScreenQuad;
    OITTriMeshPtr mModel;
};

}

#endif