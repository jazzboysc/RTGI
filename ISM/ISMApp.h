#ifndef RTGI_ISMApp_H
#define RTGI_ISMApp_H

#include "GraphicsFrameworkHeader.h"
#include "ISMTriMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/05/2014
//----------------------------------------------------------------------------
class ISMApp : public Application
{
public:
	ISMApp(int width, int height);
	~ISMApp();

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
	void DrawScene();

	int mWidth, mHeight;
	std::string mWindowTitle;

	Camera* mCamera;

	ISMTriMeshPtr mGround;
	ISMTriMeshPtr mCeiling;
	ISMTriMeshPtr mLight;
	ISMTriMeshPtr mBackWall;
	ISMTriMeshPtr mLeftWall;
	ISMTriMeshPtr mRightWall;
	ISMTriMeshPtr mModel;
};

}

#endif