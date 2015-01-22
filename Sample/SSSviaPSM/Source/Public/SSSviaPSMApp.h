#ifndef RTGI_SSSviaPSMApp_H
#define RTGI_SSSviaPSMApp_H

#include "SSSviaPSMTriMesh.h"
#include "SSSviaPSMTempScreenQuad.h"

namespace RTGI
{

//----------------------------------------------------------------------------
// Author: Che Sun
// Date: 09/09/2014
//----------------------------------------------------------------------------
class SSSviaPSMApp : public Application
{
public:
	SSSviaPSMApp(int width = 768, int height = 768);
	~SSSviaPSMApp();

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

    bool mIsRotatingModel;
    bool mIsWireframe;
    ShowMode mShowMode;

	void DrawShadow();
    void DrawScene();

    Camera* mLightProjector;

    Texture2DPtr mShadowMapTexture;
    Texture2DPtr mShadowMapDepthTexture;
    FrameBufferPtr mShadowMapFB;

    SSSviaPSMTempScreenQuadPtr mShadowMapScreenQuad;

	SSSviaPSMTriMeshPtr mGround;
	SSSviaPSMTriMeshPtr mCeiling;
	SSSviaPSMTriMeshPtr mBackWall;
	SSSviaPSMTriMeshPtr mLeftWall;
	SSSviaPSMTriMeshPtr mRightWall;
	SSSviaPSMTriMeshPtr mModel;
};

}

#endif