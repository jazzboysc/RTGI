#ifndef RTGI_ISMApp_H
#define RTGI_ISMApp_H

#include "ISMTriMesh.h"
#include "ISMTempScreenQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/05/2014
//----------------------------------------------------------------------------
class ISMApp : public Application
{
public:
	ISMApp(int width = 1024, int height = 768);
	~ISMApp();

	void Initialize(GPUDevice* device) override;
	void ProcessInput() override;
	void FrameFunc() override;
	void Terminate() override;

private:
    enum ShowMode
    {
        SM_Shadow,
        SM_Scene
    };

    bool mIsWireframe;
    ShowMode mShowMode;

	void DrawShadow();
    void DrawScene();

    Camera* mLightProjector;

    Texture2DPtr mShadowMapTexture;
    Texture2DPtr mShadowMapDepthTexture;
    FrameBufferPtr mShadowMapFB;

    ISMTempScreenQuadPtr mShadowMapScreenQuad;

	ISMTriMeshPtr mGround;
	ISMTriMeshPtr mCeiling;
	ISMTriMeshPtr mBackWall;
	ISMTriMeshPtr mLeftWall;
	ISMTriMeshPtr mRightWall;
	ISMTriMeshPtr mModel;
};

}

#endif