#ifndef RTGI_SimpleVoxelizationApp_H
#define RTGI_SimpleVoxelizationApp_H

#include "GraphicsFrameworkHeader.h"
#include "SimpleVoxelizationTriMesh.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 10/08/2014
//----------------------------------------------------------------------------
class SimpleVoxelizationApp : public Application
{
public:
	SimpleVoxelizationApp(int width, int height);
	~SimpleVoxelizationApp();

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
	void VoxelizeScene();

	int mWidth, mHeight;
	std::string mWindowTitle;

	Camera* mCamera;

    enum { VOXEL_DIMENSION = 256 };
    Texture3DPtr mSceneVoxels;

	SimpleVoxelizationTriMeshPtr mGround;
	SimpleVoxelizationTriMeshPtr mCeiling;
	SimpleVoxelizationTriMeshPtr mBackWall;
	SimpleVoxelizationTriMeshPtr mLeftWall;
	SimpleVoxelizationTriMeshPtr mRightWall;
	SimpleVoxelizationTriMeshPtr mModel;
};

}

#endif