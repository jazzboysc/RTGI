#ifndef RTGI_CornellApp_H
#define RTGI_CornellApp_H

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
	CornellApp(int width = 768, int height = 768);
	~CornellApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;

private:
	void DrawScene();

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