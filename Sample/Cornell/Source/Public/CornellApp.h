#ifndef RTGI_CornellApp_H
#define RTGI_CornellApp_H

#include "GraphicsFrameworkHeader.h"
#include "CornellTriMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/05/2014
//----------------------------------------------------------------------------
class CornellApp : public Application
{
public:
	CornellApp(int width, int height);
	~CornellApp();

	void Initialize(GPUDevice* device);
	void Run();
	void Terminate();

	// Event handlers.
	void OnKeyboard(unsigned char key, int x, int y);
	void OnKeyboardUp(unsigned char key, int x, int y);
	void OnMouse(int button, int state, int x, int y);
	void OnMouseMove(int x, int y);
	void OnReshape(int x, int y);

private:
	void DrawScene();

	int mWidth, mHeight;
	std::string mWindowTitle;

	Camera* mCamera;

	CornellTriMeshPtr mGround;
	CornellTriMeshPtr mCeiling;
	CornellTriMeshPtr mLight;
	CornellTriMeshPtr mBackWall;
	CornellTriMeshPtr mLeftWall;
	CornellTriMeshPtr mRightWall;
	CornellTriMeshPtr mModel;
};

}

#endif